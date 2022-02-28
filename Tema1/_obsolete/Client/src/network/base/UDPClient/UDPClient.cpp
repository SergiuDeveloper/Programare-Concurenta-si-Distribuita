#include "UDPClient.h"


UDPClient::UDPClient(char * ip, int port) {
    this->ip = ip;
    this->port = port;
}

void UDPClient::run() {
    sockDesc = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockDesc < 0) {
        throw std::runtime_error("Failed to open socket");
    }

    struct sockaddr_in sockAddr = {};
    sockAddr.sin_family = AF_INET;
    sockAddr.sin_addr.s_addr = inet_addr(ip);
    sockAddr.sin_port = htons(port);

    clientLogic(sockDesc, sockAddr);

    close(sockDesc);
}