#include "UDPDownloadTransmission.h"


UDPDownloadTransmission::UDPDownloadTransmission(int port, std::string benchmarkFilePath, int chunkSize, bool acknowledge) : UDPServer(port) {
    if (chunkSize > UDP_MAX_BUFFER_SIZE || chunkSize <= 0) {
        throw new std::logic_error("Invalid chunk size (1 <= CHUNK_SIZE <= 65535");
    }

    this->benchmarkChunks = getBenchmarkChunks(benchmarkFilePath, chunkSize);
    this->benchmarkChunksMutex = new std::mutex();
    this->chunkSize = chunkSize;
    this->acknowledge = acknowledge;
    this->chunkCounterMutex = new std::mutex();
}

UDPDownloadTransmission::~UDPDownloadTransmission() {
    delete this->benchmarkChunksMutex;
    delete this->chunkCounterMutex;
}

void UDPDownloadTransmission::serverLogic() {
    struct sockaddr_in clientSockAddr;
    socklen_t clientSockAddrSize = sizeof(clientSockAddr);
    uint8_t * readBuffer = new uint8_t[1];
    int readBytes;
    while (getRunning()) {
        readBytes = recvfrom(getSockDesc(), readBuffer, 1, 0, (struct sockaddr *)&clientSockAddr, &clientSockAddrSize);
        if (readBytes <= 0) {
            std::cerr<<"Failed to receive client request";
            continue;
        }

        char * clientIP = inet_ntoa(clientSockAddr.sin_addr);
        if (chunkCounter.find(clientIP) == chunkCounter.end()) {
            setChunkCounterValue(clientIP, 0);
        }

        provideBenchmarkChunk(clientSockAddr, clientIP);
    }
}

void UDPDownloadTransmission::provideBenchmarkChunk(struct sockaddr_in clientSockAddr, char * clientIP) {
    std::vector<unsigned char> benchmarkChunk = benchmarkChunks.at(chunkCounter[clientIP]);
    unsigned char * benchmarkChunkArr = &benchmarkChunks.at(chunkCounter[clientIP])[0];
    sendto(getSockDesc(), benchmarkChunkArr, benchmarkChunk.size(), 0, (struct sockaddr *)&clientSockAddr, sizeof(clientSockAddr));

    if (chunkCounter[clientIP] == benchmarkChunks.size() - 1) {
        return;
    }
    setChunkCounterValue(clientIP, chunkCounter[clientIP] + 1);
}

std::vector<std::vector<unsigned char>> UDPDownloadTransmission::getBenchmarkChunks(std::string benchmarkFilePath, int chunkSize) {
    std::vector<std::vector<unsigned char>> chunks;

    std::ifstream benchmarkFile(benchmarkFilePath, std::ios::binary);
    std::vector<unsigned char> benchmarkFileContent = std::vector<unsigned char>(std::istreambuf_iterator<char>(benchmarkFile), {});
    
    for (size_t i = 0; i < benchmarkFileContent.size(); i += chunkSize) {
        auto last = std::min(benchmarkFileContent.size(), i + chunkSize);
        chunks.emplace_back(benchmarkFileContent.begin() + i, benchmarkFileContent.begin() + last);
    }

    return chunks;
}

void UDPDownloadTransmission::setChunkCounterValue(char * clientIP, int chunkIndex) {
    chunkCounterMutex->lock();
    chunkCounter[clientIP] = chunkIndex;
    chunkCounterMutex->unlock();   
}