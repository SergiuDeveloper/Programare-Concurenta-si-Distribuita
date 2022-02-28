#include "TCPUploadTransmissionClient.h"


TCPUploadTransmissionClient::TCPUploadTransmissionClient(TimestampsHandler * timestampsHandler, char * ip, int port, int chunkSize, std::string benchmarkFilePath, bool acknowledge) : TCPClient(ip, port) {
    if (chunkSize > TCP_MAX_BUFFER_SIZE || chunkSize <= 0) {
        throw new std::logic_error("Invalid chunk size (1 <= CHUNK_SIZE <= 65535)");
    }

    this->timestampsHandler = timestampsHandler;
    this->benchmarkChunks = getBenchmarkChunks(benchmarkFilePath, chunkSize);
    this->chunkSize = chunkSize;
    this->benchmarkFilePath = benchmarkFilePath;
    this->acknowledge = acknowledge;
}

void TCPUploadTransmissionClient::clientLogic(int sockDesc) {
    int benchmarkTotalBytes = (benchmarkChunks.size() - 1) * benchmarkChunks.at(0).size() + benchmarkChunks.at(benchmarkChunks.size() - 1).size();
    
    this->timestampsHandler->setTimestamp(std::time(nullptr));
    this->timestampsHandler->setBytesCount(benchmarkTotalBytes);

    uint8_t * readBuffer = new uint8_t[1];
    int readBytes;
    unsigned char * benchmarkChunkArr;
    for (std::vector<unsigned char> benchmarkChunk: benchmarkChunks) {
        benchmarkChunkArr = &benchmarkChunk[0];
        send(sockDesc, benchmarkChunkArr, benchmarkChunk.size(), 0);

        if (acknowledge) {
            readBytes = read(sockDesc, readBuffer, 1);
            
            if (readBytes < 0) {
                std::cerr<<"Failed to read from socket\r\n";
                continue;
            } else if (readBytes == 0) {
                std::cerr<<"Connection closed unexpectedly\r\n";
                break;
            }
        }
    }

    if (acknowledge) {
        this->timestampsHandler->tcpUploadDone();
    } else {
        this->timestampsHandler->tcpUploadStreamDone();
    }
}

std::vector<std::vector<unsigned char>> TCPUploadTransmissionClient::getBenchmarkChunks(std::string benchmarkFilePath, int chunkSize) {
    std::vector<std::vector<unsigned char>> chunks;

    std::ifstream benchmarkFile(benchmarkFilePath, std::ios::binary);
    std::vector<unsigned char> benchmarkFileContent = std::vector<unsigned char>(std::istreambuf_iterator<char>(benchmarkFile), {});
    
    for (size_t i = 0; i < benchmarkFileContent.size(); i += chunkSize) {
        auto last = std::min(benchmarkFileContent.size(), i + chunkSize);
        chunks.emplace_back(benchmarkFileContent.begin() + i, benchmarkFileContent.begin() + last);
    }

    return chunks;
}