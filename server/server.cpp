#include "server.h"

Server::Server()
{
    if(this->listen(QHostAddress::Any, 2323))
    {
        qDebug()<<"start";
    }
    else{
        qDebug()<<"error";
    }
    nextBlockSize=0;

}

void Server::incomingConnection(qintptr socketDescriptor)
{
    socket = new QTcpSocket();
    socket->setSocketDescriptor(socketDescriptor);
    connect(socket, &QTcpSocket::readyRead, this, &Server::slotReadyRead);
    connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);

    sockets.push_back(socket);
    qDebug()<<"client connected"<<socketDescriptor;
}

void Server::slotReadyRead()
{
    socket = (QTcpSocket*)sender();
    QDataStream in (socket);
    in.setVersion(QDataStream::Qt_6_2);
    if(in.status() == QDataStream::Ok)
    {

        while(true)
        {
            if(nextBlockSize==0)
            {
                if(socket->bytesAvailable()<2)
                {
                    qDebug()<<"Date<2 ,break";
                    break;
                }
                in>>nextBlockSize;
            }
            if(socket->bytesAvailable()<nextBlockSize)
            {
                qDebug()<<"Data not full";
                break;
            }
            QString str;
            QTime time;
            in>>time>>str;
            nextBlockSize=0;
            sendToClient(str);
            break;
        }
    }
    else
    {
        qDebug()<<"DataStream error";
    }

}

void Server::sendToClient(QString str)
{
    data.clear();
    QDataStream out(&data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_2);
    out<<quint16(0)<<QTime::currentTime()<<str;
    out.device()->seek(0);
    out<<quint16(data.size()-sizeof(quint16));

    for(auto i: sockets)
    {
        i->write(data);
    }
}
