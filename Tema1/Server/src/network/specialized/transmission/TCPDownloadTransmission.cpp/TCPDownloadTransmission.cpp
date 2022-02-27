#include "TCPDownloadTransmission.h"


TCPDownloadTransmission::TCPDownloadTransmission(int port, std::string benchmarkFilePath, int chunkSize, bool acknowledge) : TCPServer(port) {
    if (chunkSize > TCP_MAX_BUFFER_SIZE || chunkSize <= 0) {
        throw new std::logic_error("Invalid chunk size (1 <= CHUNK_SIZE <= 65535");
    }

    this->benchmarkChunks = getBenchmarkChunks(benchmarkFilePath, chunkSize);
    this->chunkSize = chunkSize;
    this->acknowledge = acknowledge;
}

void TCPDownloadTransmission::handleClient(int clientSockDesc, char * clientIP) {
    sendBenchmarkFile(clientSockDesc, clientIP);
}

void TCPDownloadTransmission::sendBenchmarkFile(int clientSockDesc, char * clientIP) {
    uint8_t * readBuffer = new uint8_t[1];
    int readBytes;
    unsigned char * benchmarkChunkArr;
    for (std::vector<unsigned char> benchmarkChunk: benchmarkChunks) {
        benchmarkChunkArr = &benchmarkChunk[0];
        send(clientSockDesc, benchmarkChunkArr, benchmarkChunk.size(), 0);

        if (acknowledge) {
            readBytes = read(clientSockDesc, readBuffer, 1);
            
            if (readBytes < 0) {
                std::cerr<<"Failed to read from socket";
                continue;
            } else if (readBytes == 0) {
                std::cerr<<"Connection closed unexpectedly";
                break;
            }
        }
    }
}

std::vector<std::vector<unsigned char>> TCPDownloadTransmission::getBenchmarkChunks(std::string benchmarkFilePath, int chunkSize) {
    std::vector<std::vector<unsigned char>> chunks;

    std::ifstream benchmarkFile(benchmarkFilePath, std::ios::binary);
    std::vector<unsigned char> benchmarkFileContent = std::vector<unsigned char>(std::istreambuf_iterator<char>(benchmarkFile), {});
    
    for (size_t i = 0; i < benchmarkFileContent.size(); i += chunkSize) {
        auto last = std::min(benchmarkFileContent.size(), i + chunkSize);
        chunks.emplace_back(benchmarkFileContent.begin() + i, benchmarkFileContent.begin() + last);
    }

    return chunks;
}