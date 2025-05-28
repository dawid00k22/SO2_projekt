#include <iostream>
#include <thread>
#include <string>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

#define PORT 8080

void receive_messages(SOCKET sock) {
    char buffer[1024];
    while (true) {
        int bytes_received = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received <= 0) break;
        buffer[bytes_received] = '\0';
        std::cout << "\n" << buffer << "\n> ";
        std::cout.flush();
    }
}

int main() {
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    connect(sock, (sockaddr*)&serv_addr, sizeof(serv_addr));

    char buffer[1024];
    int len = recv(sock, buffer, sizeof(buffer) - 1, 0);
    if (len > 0) {
        buffer[len] = '\0';
        std::cout << buffer;
    }

    std::string nickname;
    std::getline(std::cin, nickname);
    send(sock, nickname.c_str(), nickname.length(), 0);

    std::cout << "Connected as [" << nickname << "]. Type /help for commands.\n";

    std::thread receiver(receive_messages, sock);
    receiver.detach();

    std::string message;
    while (true) {
        std::cout << "> ";
        std::getline(std::cin, message);
        if (message == "/quit") break;
        send(sock, message.c_str(), message.length(), 0);
    }

    closesocket(sock);
    WSACleanup();
    return 0;
}
