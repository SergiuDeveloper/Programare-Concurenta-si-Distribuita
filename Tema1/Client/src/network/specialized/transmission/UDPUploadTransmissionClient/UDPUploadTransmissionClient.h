#ifndef UDP_UPLOAD_TRANSMISSION_CLIENT
#define UDP_UPLOAD_TRANSMISSION_CLIENT

#include <iostream>
#include <fstream>
#include <vector>

#include "../../../base/UDPClient/UDPClient.h"


class UDPUploadTransmissionClient : public UDPClient {
private:
    std::vector<std::vector<unsigned char>> benchmarkChunks;
    int chunkSize;
    std::string benchmarkFilePath;
    bool acknowledge;

public:
    UDPUploadTransmissionClient(char * ip, int port, int chunkSize, std::string benchmarkFilePath, bool acknowledge);

private:
    void clientLogic(int sockDesc, struct sockaddr_in serverSockAddr);
    std::vector<std::vector<unsigned char>> getBenchmarkChunks(std::string benchmarkFilePath, int chunkSize);
};

#endif