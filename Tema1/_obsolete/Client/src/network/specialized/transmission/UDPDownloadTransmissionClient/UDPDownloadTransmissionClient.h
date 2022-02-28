#ifndef UDP_DOWNLOAD_TRANSMISSION_CLIENT
#define UDP_DOWNLOAD_TRANSMISSION_CLIENT

#include <iostream>
#include <fstream>
#include <cstring>

#include "../../../base/UDPClient/UDPClient.h"
#include "../../control/TimestampsHandler/TimestampsHandler.h"


class UDPDownloadTransmissionClient : public UDPClient {
private:
    TimestampsHandler * timestampsHandler;
    int chunkSize;
    std::string benchmarkFilePath;
    bool acknowledge;

public:
    UDPDownloadTransmissionClient(TimestampsHandler * timestampsHandler, char * ip, int port, int chunkSize, std::string benchmarkFilePath, bool acknowledge);

private:
    void clientLogic(int sockDesc, struct sockaddr_in serverSockAddr);
};

#endif