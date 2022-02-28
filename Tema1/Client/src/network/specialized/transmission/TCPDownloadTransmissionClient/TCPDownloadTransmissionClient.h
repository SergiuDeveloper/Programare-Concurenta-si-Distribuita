#ifndef TCP_DOWNLOAD_TRANSMISSION_CLIENT
#define TCP_DOWNLOAD_TRANSMISSION_CLIENT

#include <iostream>
#include <fstream>
#include <cstring>

#include "../../../base/TCPClient/TCPClient.h"


class TCPDownloadTransmissionClient : public TCPClient {
private:
    int chunkSize;
    std::string benchmarkFilePath;
    bool acknowledge;

public:
    TCPDownloadTransmissionClient(char * ip, int port, int chunkSize, std::string benchmarkFilePath, bool acknowledge);

private:
    void clientLogic(int sockDesc);
};

#endif