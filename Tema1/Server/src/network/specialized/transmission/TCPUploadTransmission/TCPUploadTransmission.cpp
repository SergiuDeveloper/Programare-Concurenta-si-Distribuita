#include "TCPUploadTransmission.h"


TCPUploadTransmission::TCPUploadTransmission(int port, bool acknowledge) : TCPServer(port) {
    this->acknowledge = acknowledge;
}

void TCPUploadTransmission::handleClient(int clientSockDesc, char * clientIP) {
    receiveBenchmarkFile(clientSockDesc, clientIP);
}

void TCPUploadTransmission::receiveBenchmarkFile(int clientSockDesc, char * clientIP) {
    int totalReadBytes = 0;

    uint8_t * readBuffer = new uint8_t[1];
    int readBytes;
    while ((readBytes = read(clientSockDesc, readBuffer, 1)) != 0) {
        if (readBytes < 0) {
            std::cerr<<"Failed to read from socket";
            continue;
        }

        if (acknowledge) {
            send(clientSockDesc, "1", 1, 0);
        }

        totalReadBytes += readBytes;
    }
}