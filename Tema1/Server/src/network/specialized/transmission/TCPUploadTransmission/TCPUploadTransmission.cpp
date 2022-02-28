#include "TCPUploadTransmission.h"


TCPUploadTransmission::TCPUploadTransmission(TimestampsHandler * timestampsHandler, int port, int chunkSize, bool acknowledge) : TCPServer(port) {
    if (chunkSize > TCP_MAX_BUFFER_SIZE || chunkSize <= 0) {
        throw new std::logic_error("Invalid chunk size (1 <= CHUNK_SIZE <= 65535)");
    }
    
    this->timestampsHandler = timestampsHandler;
    this->chunkSize = chunkSize;
    this->acknowledge = acknowledge;
}

void TCPUploadTransmission::handleClient(int clientSockDesc, char * clientIP) {
    receiveBenchmarkFile(clientSockDesc, clientIP);
}

void TCPUploadTransmission::receiveBenchmarkFile(int clientSockDesc, char * clientIP) {
    int totalReadBytes = 0;

    uint8_t * readBuffer = new uint8_t[chunkSize];
    int readBytes;
    while ((readBytes = read(clientSockDesc, readBuffer, chunkSize)) != 0) {
        if (readBytes < 0) {
            std::cerr<<"Failed to read from socket";
            continue;
        }

        if (acknowledge) {
            send(clientSockDesc, "1", 1, 0);
        }

        totalReadBytes += readBytes;
    }

    this->timestampsHandler->setTimestamp(clientIP, std::time(nullptr));
    this->timestampsHandler->setBytesCount(clientIP, totalReadBytes);
}