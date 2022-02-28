#ifndef UDP_DOWNLOAD_TRANSMISSION_CLIENT
#define UDP_DOWNLOAD_TRANSMISSION_CLIENT

#include <iostream>
#include <fstream>
#include <cstring>

#include "../../../base/UDPClient/UDPClient.h"


class UDPDownloadTransmissionClient : public UDPClient {
private:
    int chunkSize;
    std::string benchmarkFilePath;
    bool acknowledge;

public:
    UDPDownloadTransmissionClient(char * ip, int port, int chunkSize, std::string benchmarkFilePath, bool acknowledge);

private:
    void clientLogic(int sockDesc, struct sockaddr_in serverSockAddr);
};

#endif