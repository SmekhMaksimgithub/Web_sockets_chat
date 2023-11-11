// SkillChatv2.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//
#include "../include/main.h"

// Public messages
// user1 => server : {"command":"public_message", "text": "Hello everyone"}
// server => all users {"command" : "public_message", "text": "...", user_from: 1};
// data, ws, 
void process_public_msg(nlohmann::json data, auto* ws)
{
   
    nlohmann::json payload = {
        {"command", data["command"]},
        {"text", data["text"]},
        {"user_from",ws->getUserData()->user_id }
    };
    ws->publish("public", payload.dump());


}
// Private messages
// user1 => server {"command":"private_message message", "text": "...", user_to :2}
// server => user2 {"command" : "private_message", "text": "...", user_from: 1};
void process_private_msg(nlohmann::json data, auto* ws)
{
    nlohmann::json payload = {
        {"command", data["command"]},
        {"text", data["text"]},
        {"user_from",ws->getUserData()->user_id }
    };
    int user_to = data["user_to"];
    ws->publish("user" + std::to_string(user_to), payload.dump());
}



// Set name possibility


int main() {
    /* ws->getUserData returns one of these */
    struct PerSocketData {
        /* Fill with user data */
        int user_id;
        std::string name;
    };
    int latest_user_id = 0;

    /* Keep in mind that uWS::SSLApp({options}) is the same as uWS::App() when compiled without SSL support.
     * You may swap to using uWS:App() if you don't need SSL */
    uWS::App().ws<PerSocketData>("/*", {
            /* Settings */
            .idleTimeout = 16,
            /* Handlers */
            .open = [&latest_user_id](auto* ws) {
                /* Open event here, you may access ws->getUserData() which points to a PerSocketData struct */
                PerSocketData * data = ws->getUserData();
                data->user_id = latest_user_id++;
               data->name = "unknown";
               ws->subscribe("public");
               ws->subscribe("user" + std::to_string(data->user_id));
               std::cout << "New user connected: " << data->user_id << '\n';
            },
            .message = [](auto* ws, std::string_view message, uWS::OpCode opCode) {
                /* This is the opposite of what you probably want; compress if message is LARGER than 16 kb
                 * the reason we do the opposite here; compress if SMALLER than 16 kb is to allow for
                 * benchmarking of large message sending without compression*/
                nlohmann::json parsed_data = nlohmann::json::parse(message); //ToDo: check format / handle exception
                //ws->send(message, opCode, message.length() < 16 * 1024);
                if (parsed_data["command"] == "public_msg")
                {
                    process_public_msg(parsed_data, ws);
                } 
                if (parsed_data["command"] == "private_msg")
                {
                    process_private_msg(parsed_data, ws);
                }
            },
            .close = [](auto*/*ws*/, int /*code*/, std::string_view /*message*/) {
                /* You may access ws->getUserData() here */
            }
            }).listen(9001, [](auto* listen_socket) {
                if (listen_socket) {
                    std::cout << "Listening on port " << 9001 << std::endl;
                }
                }).run();
}