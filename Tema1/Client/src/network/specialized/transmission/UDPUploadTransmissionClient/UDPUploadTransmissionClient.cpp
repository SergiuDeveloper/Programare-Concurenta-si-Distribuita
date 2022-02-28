#include "UDPUploadTransmissionClient.h"


UDPUploadTransmissionClient::UDPUploadTransmissionClient(char * ip, int port, int chunkSize, std::string benchmarkFilePath, bool acknowledge) : UDPClient(ip, port) {
    if (chunkSize > UDP_MAX_BUFFER_SIZE || chunkSize <= 0) {
        throw new std::logic_error("Invalid chunk size (1 <= CHUNK_SIZE <= 65535)");
    }
    
    this->benchmarkChunks = getBenchmarkChunks(benchmarkFilePath, chunkSize);
    this->chunkSize = chunkSize;
    this->benchmarkFilePath = benchmarkFilePath;
    this->acknowledge = acknowledge;
}

void UDPUploadTransmissionClient::clientLogic(int sockDesc, struct sockaddr_in serverSockAddr) {
    int benchmarkTotalBytes = (benchmarkChunks.size() - 1) * benchmarkChunks.at(0).size() + benchmarkChunks.at(benchmarkChunks.size() - 1).size();
    
    uint8_t * readBuffer = new uint8_t[1];
    socklen_t serverSockAddrSize = sizeof(serverSockAddr);
    for (std::vector<unsigned char> benchmarkChunk: benchmarkChunks) {
        unsigned char * benchmarkChunkArr = &benchmarkChunk[0];
        sendto(sockDesc, benchmarkChunkArr, benchmarkChunk.size(), 0, (struct sockaddr *)&serverSockAddr, sizeof(serverSockAddr));
    
        if (acknowledge) {
            recvfrom(sockDesc, readBuffer, 1, 0, (struct sockaddr *)&serverSockAddr, &serverSockAddrSize);
        }
    }
}

std::vector<std::vector<unsigned char>> UDPUploadTransmissionClient::getBenchmarkChunks(std::string benchmarkFilePath, int chunkSize) {
    std::vector<std::vector<unsigned char>> chunks;

    std::ifstream benchmarkFile(benchmarkFilePath, std::ios::binary);
    std::vector<unsigned char> benchmarkFileContent = std::vector<unsigned char>(std::istreambuf_iterator<char>(benchmarkFile), {});
    
    for (size_t i = 0; i < benchmarkFileContent.size(); i += chunkSize) {
        auto last = std::min(benchmarkFileContent.size(), i + chunkSize);
        chunks.emplace_back(benchmarkFileContent.begin() + i, benchmarkFileContent.begin() + last);
    }

    return chunks;
}