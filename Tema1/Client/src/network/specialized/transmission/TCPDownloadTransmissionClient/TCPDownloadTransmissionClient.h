#ifndef TCP_DOWNLOAD_TRANSMISSION_CLIENT
#define TCP_DOWNLOAD_TRANSMISSION_CLIENT

#include <iostream>
#include <fstream>
#include <cstring>

#include "../../../base/TCPClient/TCPClient.h"
#include "../../control/TimestampsHandler/TimestampsHandler.h"


class TCPDownloadTransmissionClient : public TCPClient {
private:
    TimestampsHandler * timestampsHandler;
    int chunkSize;
    std::string benchmarkFilePath;
    bool acknowledge;

public:
    TCPDownloadTransmissionClient(TimestampsHandler * timestampsHandler, char * ip, int port, int chunkSize, std::string benchmarkFilePath, bool acknowledge);

private:
    void clientLogic(int sockDesc);
};

#endif