#include "UDPDownloadTransmissionClient.h"


UDPDownloadTransmissionClient::UDPDownloadTransmissionClient(char * ip, int port, int chunkSize, std::string benchmarkFilePath, bool acknowledge) : UDPClient(ip, port) {
    if (chunkSize > UDP_MAX_BUFFER_SIZE || chunkSize <= 0) {
        throw new std::logic_error("Invalid chunk size (1 <= CHUNK_SIZE <= 65535)");
    }
    
    this->chunkSize = chunkSize;
    this->benchmarkFilePath = benchmarkFilePath;
    this->acknowledge = acknowledge;
}

void UDPDownloadTransmissionClient::clientLogic(int sockDesc, struct sockaddr_in serverSockAddr) {
    std::ofstream benchmarkFile(benchmarkFilePath, std::ios::binary);

    uint8_t * readBuffer = new uint8_t[chunkSize];
    int readBytes;
    int totalReadBytes = 0;

    sendto(sockDesc, "1", 1, 0, (struct sockaddr *)&serverSockAddr, sizeof(serverSockAddr));

    socklen_t serverSockAddrSize = sizeof(serverSockAddr);
    while ((readBytes = recvfrom(sockDesc, readBuffer, chunkSize, 0, (struct sockaddr *)&serverSockAddr, &serverSockAddrSize)) != 1) {
        if (readBytes <= 0) {
            std::cerr<<"Failed to read from socket\r\n";
            continue;
        }

        uint8_t * writeFileBuffer = new uint8_t[readBytes];
        memcpy(writeFileBuffer, readBuffer, readBytes);
        benchmarkFile<<writeFileBuffer;

        totalReadBytes += readBytes;

        if (acknowledge) {
            sendto(sockDesc, "1", 1, 0, (struct sockaddr *)&serverSockAddr, sizeof(serverSockAddr));
        }
    }

    benchmarkFile.close();
}