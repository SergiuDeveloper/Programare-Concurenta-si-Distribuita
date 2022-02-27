#include "TCPDownloadTransmission.h"


TCPDownloadTransmission::TCPDownloadTransmission(TimestampsHandler * timestampsHandler, int port, std::string benchmarkFilePath, int chunkSize, bool acknowledge) : TCPServer(port) {
    if (chunkSize > TCP_MAX_BUFFER_SIZE || chunkSize <= 0) {
        throw new std::logic_error("Invalid chunk size (1 <= CHUNK_SIZE <= 65535");
    }

    this->timestampsHandler = timestampsHandler;
    this->benchmarkChunks = getBenchmarkChunks(benchmarkFilePath, chunkSize);
    this->chunkSize = chunkSize;
    this->acknowledge = acknowledge;
}

void TCPDownloadTransmission::handleClient(int clientSockDesc, char * clientIP) {
    sendBenchmarkFile(clientSockDesc, clientIP);
}

void TCPDownloadTransmission::sendBenchmarkFile(int clientSockDesc, char * clientIP) {
    int benchmarkTotalBytes = (benchmarkChunks.size() - 1) * benchmarkChunks.at(0).size() + benchmarkChunks.at(benchmarkChunks.size() - 1).size();
    
    this->timestampsHandler->setTimestamp(clientIP, std::time(nullptr));
    this->timestampsHandler->setBytesCount(clientIP, benchmarkTotalBytes);
    
    uint8_t * readBuffer = new uint8_t[1];
    int readBytes;
    unsigned char * benchmarkChunkArr;
    for (std::vector<unsigned char> benchmarkChunk: benchmarkChunks) {
        benchmarkChunkArr = &benchmarkChunk[0];
        send(clientSockDesc, benchmarkChunkArr, benchmarkChunk.size(), 0);

        if (acknowledge) {
            readBytes = read(clientSockDesc, readBuffer, 1);
            
            if (readBytes < 0) {
                std::cerr<<"Failed to read from socket\r\n";
                continue;
            } else if (readBytes == 0) {
                std::cerr<<"Connection closed unexpectedly\r\n";
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