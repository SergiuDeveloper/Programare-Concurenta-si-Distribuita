#ifndef TCP_DOWNLOAD_TRANSMISSION
#define TCP_DOWNLOAD_TRANSMISSION

#define TCP_MAX_BUFFER_SIZE 65535

#include <iostream>
#include <fstream>
#include <iterator>
#include <vector>

#include "../../../base/TCPServer/TCPServer.h"


class TCPDownloadTransmission : private TCPServer {
private:
    std::vector<std::vector<unsigned char>> benchmarkChunks;
    int chunkSize;
    bool acknowledge;

public:
    TCPDownloadTransmission(int port, std::string benchmarkFilePath, int chunkSize, bool acknowledge);

private:
    void handleClient(int clientSockDesc, char * clientIP);
    void sendBenchmarkFile(int clientSockDesc, char * clientIP);
    std::vector<std::vector<unsigned char>> getBenchmarkChunks(std::string benchmarkFilePath, int chunkSize);
};

#endif