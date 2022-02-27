#ifndef CONTROL_SERVER
#define CONTROL_SERVER

#define SLEEP_TIME_BEFORE_UPLOAD_RESPOND 1 * 1000 * 1000
#define BENCHMARKS_COUNT 8

#include <iostream>
#include <set>
#include <unordered_map>
#include <mutex>
#include <ctime>

#include "../../../base/TCPServer/TCPServer.h"
#include "../TimestampsHandler/TimestampsHandler.h"
#include "../../transmission/TCPDownloadTransmission/TCPDownloadTransmission.h"
#include "../../transmission/TCPUploadTransmission/TCPUploadTransmission.h"
#include "../../transmission/UDPDownloadTransmission/UDPDownloadTransmission.h"
#include "../../transmission/UDPUploadTransmission/UDPUploadTransmission.h"


class ControlServer : public TCPServer, public TimestampsHandler {
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
    std::mutex * activeClientsMutex;

public:
    ControlServer(int port, int tcpDownloadServerPort, int tcpUploadServerPort, int udpDownloadServerPort, int udpUploadServerPort, int tcpDownloadStreamServerPort, int tcpUploadStreamServerPort, int udpDownloadStreamServerPort, int udpUploadStreamServerPort, std::string benchmarkFilePath, int chunkSize);
    ~ControlServer();
    void run();
    void stop();

private:
    void handleClient(int clientSockDesc, char * clientIP);
    void sendPorts(int clientSockDesc, char * clientIP);
    bool satisfyRequest(int clientSockDesc, char * clientIP, bool uploadOperation);
};

#endif