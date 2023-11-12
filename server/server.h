#ifndef SERVER_H
#define SERVER_H
#include <QTcpServer>
#include <QTcpSocket>
#include <QVector>
#include <QTime>

class Server:public QTcpServer
{
    Q_OBJECT
public:
    Server();
    void sendToClient(QString);
    QTcpSocket* socket;
private:
    QVector<QTcpSocket*> sockets;
    QByteArray data;
    void SendToClient(QString);
    quint16 nextBlockSize=0;

public slots:
    void incomingConnection(qintptr socketDescriptor);
    void slotReadyRead();
};
#endif // SERVER_H
