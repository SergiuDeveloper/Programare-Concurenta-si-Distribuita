#ifndef UDP_DOWNLOAD_TRANSMISSION
#define UDP_DOWNLOAD_TRANSMISSION

#include <iostream>
#include <unordered_map>
#include <fstream>
#include <iterator>
#include <vector>

#include "../../../base/UDPServer/UDPServer.h"
#include "../../control/TimestampsHandler/TimestampsHandler.h"


class UDPDownloadTransmission : public UDPServer {
private:
    TimestampsHandler * timestampsHandler;
    std::vector<std::vector<unsigned char>> benchmarkChunks;
    std::mutex * benchmarkChunksMutex;
    int chunkSize;
    bool acknowledge;
    std::unordered_map<char *, int> chunkCounter;
    std::mutex * chunkCounterMutex;

public:
    UDPDownloadTransmission(TimestampsHandler * timestampsHandler, int port, std::string benchmarkFilePath, int chunkSize, bool acknowledge);
    ~UDPDownloadTransmission();

private:
    void serverLogic();
    void provideAllBenchmarkChunks(struct sockaddr_in clientSockAddr, char * clientIP);
    void provideBenchmarkChunk(struct sockaddr_in clientSockAddr, char * clientIP);
    std::vector<std::vector<unsigned char>> getBenchmarkChunks(std::string benchmarkFilePath, int chunkSize);
    void setChunkCounterValue(char * clientIP, int chunkIndex);
};

#endif