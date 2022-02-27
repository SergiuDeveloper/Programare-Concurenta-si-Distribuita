#include "UDPUploadTransmission.h"


UDPUploadTransmission::UDPUploadTransmission(TimestampsHandler * timestampsHandler, int port, int chunkSize, bool acknowledge) : UDPServer(port) {
    if (chunkSize > UDP_MAX_BUFFER_SIZE || chunkSize <= 0) {
        throw new std::logic_error("Invalid chunk size (1 <= CHUNK_SIZE <= 65535");
    }

    this->timestampsHandler = timestampsHandler;
    this->chunkSize = chunkSize;
    this->acknowledge = acknowledge;
    this->byteCounterMutex = new std::mutex();
}

UDPUploadTransmission::~UDPUploadTransmission() {
    delete this->byteCounterMutex;
}

void UDPUploadTransmission::serverLogic() {
    struct sockaddr_in clientSockAddr;
    socklen_t clientSockAddrSize = sizeof(clientSockAddr);
    uint8_t * readBuffer = new uint8_t[chunkSize];
    int readBytes;
    while (getRunning()) {
        readBytes = recvfrom(getSockDesc(), readBuffer, chunkSize, 0, (struct sockaddr *)&clientSockAddr, &clientSockAddrSize);
        if (readBytes <= 0) {
            std::cerr<<"Failed to receive client request\r\n";
            continue;
        }

        char * clientIP = inet_ntoa(clientSockAddr.sin_addr);

        if (byteCounter.find(clientIP) == byteCounter.end()) {
            setByteCounterValue(clientIP, readBytes);
        } else {
            setByteCounterValue(clientIP, byteCounter[clientIP] + readBytes);
        }

        if (acknowledge) {
            sendto(getSockDesc(), "1", 1, 0, (struct sockaddr *)&clientSockAddr, sizeof(clientSockAddr));
        }

        this->timestampsHandler->setTimestamp(clientIP, std::time(nullptr));
        this->timestampsHandler->setBytesCount(clientIP, byteCounter[clientIP]);
    }
}

void UDPUploadTransmission::setByteCounterValue(char * clientIP, int byteIndex) {
    byteCounterMutex->lock();
    byteCounter[clientIP] = byteIndex;
    byteCounterMutex->unlock();   
}