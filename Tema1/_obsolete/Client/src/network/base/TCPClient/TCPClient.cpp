#include "TCPClient.h"


TCPClient::TCPClient(char * ip, int port) {
    this->ip = ip;
    this->port = port;
}

void TCPClient::run() {
    sockDesc = socket(AF_INET, SOCK_STREAM, 0);
    if (sockDesc < 0) {
        throw std::runtime_error("Failed to open socket");
    }

    struct sockaddr_in sockAddr = {};
    sockAddr.sin_family = AF_INET;
    sockAddr.sin_addr.s_addr = inet_addr(ip);
    sockAddr.sin_port = htons(port);

    int connectRes = connect(sockDesc, (struct sockaddr *)&sockAddr, sizeof(sockAddr));
    if (connectRes < 0) {
        throw std::runtime_error("Failed to connect");
    }

    clientLogic(sockDesc);

    close(sockDesc);
}