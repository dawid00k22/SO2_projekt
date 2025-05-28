#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

#define PORT 8080
#define MAX_CLIENTS 10

volatile long message_lock = false;

struct ClientInfo {
    SOCKET socket;
    std::string nick;
};

std::vector<ClientInfo> clients;

void acquire_lock() {
    while (_InterlockedExchange(&message_lock, 1)) {}
}
void release_lock() {
    _InterlockedExchange(&message_lock, 0);
}

void broadcast_message(const std::string& message, SOCKET sender_socket) {
    acquire_lock();
    for (const auto& c : clients) {
        if (c.socket != sender_socket) {
            send(c.socket, message.c_str(), message.length(), 0);
        }
    }
    release_lock();
}

void handle_client(ClientInfo client) {
    char buffer[1024];
    while (true) {
        int bytes_received = recv(client.socket, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received <= 0) {
            std::cout << client.nick << " disconnected.\n";
            break;
        }
        buffer[bytes_received] = '\0';
        std::string message = buffer;

        if (message[0] == '/') {
            if (message == "/list") {
                std::string list_msg = "Online users:\n";
                acquire_lock();
                for (const auto& c : clients) {
                    list_msg += "- " + c.nick + "\n";
                }
                release_lock();
                send(client.socket, list_msg.c_str(), list_msg.length(), 0);

            } else if (message.substr(0, 8) == "/whisper") {
                size_t nick_start = 9;
                size_t space = message.find(' ', nick_start);
                if (space != std::string::npos) {
                    std::string target_nick = message.substr(nick_start, space - nick_start);
                    std::string whisper_msg = message.substr(space + 1);

                    acquire_lock();
                    SOCKET target_socket = INVALID_SOCKET;
                    for (const auto& c : clients) {
                        if (c.nick == target_nick) {
                            target_socket = c.socket;
                            break;
                        }
                    }
                    release_lock();

                    if (target_socket != INVALID_SOCKET) {
                        std::string msg_to_send = "[whisper from " + client.nick + "] " + whisper_msg;
                        send(target_socket, msg_to_send.c_str(), msg_to_send.length(), 0);
                        send(client.socket, "[whisper sent]\n", 16, 0);
                    } else {
                        send(client.socket, "User not found\n", 15, 0);
                    }
                } else {
                    send(client.socket, "Usage: /whisper nick message\n", 29, 0);
                }

            } else if (message == "/help") {
                std::string help_msg = "Commands:\n"
                                       "/list - show users\n"
                                       "/whisper nick message - private msg\n"
                                       "/help - show this help\n";
                send(client.socket, help_msg.c_str(), help_msg.length(), 0);

            } else {
                send(client.socket, "Unknown command\n", 16, 0);
            }

        } else {
            std::string full_msg = "[" + client.nick + "] " + message;
            std::cout << full_msg << "\n";
            broadcast_message(full_msg, client.socket);
        }
    }

    closesocket(client.socket);

    acquire_lock();
    for (auto it = clients.begin(); it != clients.end(); ++it) {
        if (it->socket == client.socket) {
            clients.erase(it);
            break;
        }
    }
    release_lock();
}

int main() {
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    SOCKET server_fd;
    sockaddr_in address;
    int addrlen = sizeof(address);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    bind(server_fd, (sockaddr*)&address, sizeof(address));
    listen(server_fd, 3);

    std::cout << "Server started. Waiting for connections...\n";

    while (true) {
        SOCKET new_socket = accept(server_fd, (sockaddr*)&address, &addrlen);
        if (new_socket == INVALID_SOCKET) continue;

        // Pytanie o nick
        send(new_socket, "Enter your nickname: ", 21, 0);
        char nick_buffer[64];
        int nick_len = recv(new_socket, nick_buffer, sizeof(nick_buffer) - 1, 0);
        if (nick_len <= 0) {
            closesocket(new_socket);
            continue;
        }
        nick_buffer[nick_len] = '\0';
        std::string nick = nick_buffer;

        ClientInfo client;
        client.socket = new_socket;
        client.nick = nick;

        acquire_lock();
        if (clients.size() < MAX_CLIENTS) {
            clients.push_back(client);
            std::cout << nick << " joined the chat.\n";
            std::thread t(handle_client, client);
            t.detach();
        } else {
            std::string msg = "Server full.\n";
            send(new_socket, msg.c_str(), msg.length(), 0);
            closesocket(new_socket);
        }
        release_lock();
    }

    closesocket(server_fd);
    WSACleanup();
    return 0;
}
