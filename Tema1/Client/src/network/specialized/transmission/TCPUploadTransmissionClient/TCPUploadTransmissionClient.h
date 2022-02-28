#ifndef TCP_UPLOAD_TRANSMISSION_CLIENT
#define TCP_UPLOAD_TRANSMISSION_CLIENT

#include <iostream>
#include <fstream>
#include <vector>

#include "../../../base/TCPClient/TCPClient.h"
#include "../../control/TimestampsHandler/TimestampsHandler.h"


class TCPUploadTransmissionClient : public TCPClient {
private:
    TimestampsHandler * timestampsHandler;
    std::vector<std::vector<unsigned char>> benchmarkChunks;
    int chunkSize;
    std::string benchmarkFilePath;
    bool acknowledge;

public:
    TCPUploadTransmissionClient(TimestampsHandler * timestampsHandler, char * ip, int port, int chunkSize, std::string benchmarkFilePath, bool acknowledge);

private:
    void clientLogic(int sockDesc);
    std::vector<std::vector<unsigned char>> getBenchmarkChunks(std::string benchmarkFilePath, int chunkSize);
};

#endif