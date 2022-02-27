#ifndef UDP_DOWNLOAD_TRANSMISSION
#define UDP_DOWNLOAD_TRANSMISSION

#define UDP_MAX_BUFFER_SIZE 65535

#include <iostream>
#include <unordered_map>
#include <fstream>
#include <iterator>
#include <vector>

#include "../../../base/UDPServer/UDPServer.h"


class UDPDownloadTransmission : private UDPServer {
private:
    std::vector<std::vector<unsigned char>> benchmarkChunks;
    std::mutex * benchmarkChunksMutex;
    int chunkSize;
    bool acknowledge;
    std::unordered_map<char *, int> chunkCounter;
    std::mutex * chunkCounterMutex;

public:
    UDPDownloadTransmission(int port, std::string benchmarkFilePath, int chunkSize, bool acknowledge);
    ~UDPDownloadTransmission();

private:
    void serverLogic();
    void provideAllBenchmarkChunks(struct sockaddr_in clientSockAddr, char * clientIP);
    void provideBenchmarkChunk(struct sockaddr_in clientSockAddr, char * clientIP);
    std::vector<std::vector<unsigned char>> getBenchmarkChunks(std::string benchmarkFilePath, int chunkSize);
    void setChunkCounterValue(char * clientIP, int chunkIndex);
};

#endif