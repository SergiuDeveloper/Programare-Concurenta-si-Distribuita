#ifndef CONTROL_SERVER
#define CONTROL_SERVER

#include <iostream>
#include <set>

#include "../../../base/TCPServer/TCPServer.h"
#include "../../transmission/TCPDownloadTransmission/TCPDownloadTransmission.h"
#include "../../transmission/TCPUploadTransmission/TCPUploadTransmission.h"
#include "../../transmission/UDPDownloadTransmission/UDPDownloadTransmission.h"
#include "../../transmission/UDPUploadTransmission/UDPUploadTransmission.h"


class ControlServer : public TCPServer {
private:
    TCPDownloadTransmission * tcpDownloadServer;
    TCPUploadTransmission * tcpUploadServer;
    UDPDownloadTransmission * udpDownloadServer;
    UDPUploadTransmission * udpUploadServer;
    TCPDownloadTransmission * tcpDownloadStreamServer;
    TCPUploadTransmission * tcpUploadStreamServer;
    UDPDownloadTransmission * udpDownloadStreamServer;
    UDPUploadTransmission * udpUploadStreamServer;
    int tcpDownloadServerPort;
    int tcpUploadServerPort;
    int udpDownloadServerPort;
    int udpUploadServerPort;
    int tcpDownloadStreamServerPort;
    int tcpUploadStreamServerPort;
    int udpDownloadStreamServerPort;
    int udpUploadStreamServerPort;
    std::set<char *> activeClients;

public:
    ControlServer(int port, int tcpDownloadServerPort, int tcpUploadServerPort, int udpDownloadServerPort, int udpUploadServerPort, int tcpDownloadStreamServerPort, int tcpUploadStreamServerPort, int udpDownloadStreamServerPort, int udpUploadStreamServerPort, std::string benchmarkFilePath, int chunkSize);
    ~ControlServer();
    void run();
    void stop();

private:
    void handleClient(int clientSockDesc, char * clientIP);
};

#endif