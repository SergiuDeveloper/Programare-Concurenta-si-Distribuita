#include "TCPDownloadTransmissionClient.h"


TCPDownloadTransmissionClient::TCPDownloadTransmissionClient(TimestampsHandler * timestampsHandler, char * ip, int port, int chunkSize, std::string benchmarkFilePath, bool acknowledge) : TCPClient(ip, port) {
    if (chunkSize > TCP_MAX_BUFFER_SIZE || chunkSize <= 0) {
        throw new std::logic_error("Invalid chunk size (1 <= CHUNK_SIZE <= 65535)");
    }
    
    this->timestampsHandler = timestampsHandler;
    this->chunkSize = chunkSize;
    this->benchmarkFilePath = benchmarkFilePath;
    this->acknowledge = acknowledge;
}

void TCPDownloadTransmissionClient::clientLogic(int sockDesc) {
    std::ofstream benchmarkFile(benchmarkFilePath, std::ios::binary);

    uint8_t * readBuffer = new uint8_t[chunkSize];
    int readBytes;
    int totalReadBytes = 0;
    while ((readBytes = read(sockDesc, readBuffer, chunkSize)) != 0) {
        if (readBytes < 0) {
            std::cerr<<"Failed to read from socket\r\n";
            continue;
        }

        uint8_t * writeFileBuffer = new uint8_t[readBytes];
        memcpy(writeFileBuffer, readBuffer, readBytes);
        benchmarkFile<<writeFileBuffer;

        totalReadBytes += readBytes;

        if (acknowledge) {
            send(sockDesc, "1", 1, 0);
        }
    }

    benchmarkFile.close();

    this->timestampsHandler->setTimestamp(std::time(nullptr));
    this->timestampsHandler->setBytesCount(totalReadBytes);
    if (acknowledge) {
        this->timestampsHandler->tcpDownloadDone();
    } else {
        this->timestampsHandler->tcpDownloadStreamDone();
    }
}