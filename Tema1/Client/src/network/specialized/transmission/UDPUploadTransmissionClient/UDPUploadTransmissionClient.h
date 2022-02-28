#ifndef UDP_UPLOAD_TRANSMISSION_CLIENT
#define UDP_UPLOAD_TRANSMISSION_CLIENT

#include <iostream>
#include <fstream>
#include <vector>

#include "../../../base/UDPClient/UDPClient.h"
#include "../../control/TimestampsHandler/TimestampsHandler.h"


class UDPUploadTransmissionClient : public UDPClient {
private:
    TimestampsHandler * timestampsHandler;
    std::vector<std::vector<unsigned char>> benchmarkChunks;
    int chunkSize;
    std::string benchmarkFilePath;
    bool acknowledge;

public:
    UDPUploadTransmissionClient(TimestampsHandler * timestampsHandler, char * ip, int port, int chunkSize, std::string benchmarkFilePath, bool acknowledge);

private:
    void clientLogic(int sockDesc, struct sockaddr_in serverSockAddr);
    std::vector<std::vector<unsigned char>> getBenchmarkChunks(std::string benchmarkFilePath, int chunkSize);
};

#endif