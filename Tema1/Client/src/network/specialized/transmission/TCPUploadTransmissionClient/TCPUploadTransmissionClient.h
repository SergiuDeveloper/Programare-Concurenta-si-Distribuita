#ifndef TCP_UPLOAD_TRANSMISSION_CLIENT
#define TCP_UPLOAD_TRANSMISSION_CLIENT

#include <iostream>
#include <fstream>

#include "../../../base/TCPClient/TCPClient.h"


class TCPUploadTransmissionClient : public TCPClient {
private:
    std::vector<std::vector<unsigned char>> benchmarkChunks;
    int chunkSize;
    std::string benchmarkFilePath;
    bool acknowledge;

public:
    TCPUploadTransmissionClient(char * ip, int port, int chunkSize, std::string benchmarkFilePath, bool acknowledge);

private:
    void clientLogic(int sockDesc);
    std::vector<std::vector<unsigned char>> getBenchmarkChunks(std::string benchmarkFilePath, int chunkSize)
};

#endif