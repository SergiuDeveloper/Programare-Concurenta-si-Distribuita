#ifndef CONTROL_CLIENT
#define CONTROL_CLIENT

#include <iostream>

#include "../../../base/TCPClient/TCPClient.h"
#include "../../transmission/TCPDownloadTransmissionClient/TCPDownloadTransmissionClient.h"
#include "../../transmission/TCPUploadTransmissionClient/TCPUploadTransmissionClient.h"
#include "../../transmission/UDPDownloadTransmissionClient/UDPDownloadTransmissionClient.h"
#include "../../transmission/UDPUploadTransmissionClient/UDPUploadTransmissionClient.h"
#include "../TimestampsHandler/TimestampsHandler.h"

class ControlClient : public TCPClient, public TimestampsHandler {
private:
    int tcpDownloadServerPort;
    int tcpUploadServerPort;
    int udpDownloadServerPort;
    int udpUploadServerPort;
    int tcpDownloadStreamServerPort;
    int tcpUploadStreamServerPort;
    int udpDownloadStreamServerPort;
    int udpUploadStreamServerPort;
    int chunkSize;
    std::string benchmarkFilePath;
    bool done;

public:
    ControlClient(char * ip, int port, int chunkSize, std::string benchmarkFilePath);

public:
    void tcpDownloadDone();
    void tcpUploadDone();
    void udpDownloadDone();
    void udpUploadDone();
    void tcpDownloadStreamDone();
    void tcpUploadStreamDone();
    void udpDownloadStreamDone();
    void udpUploadStreamDone();

    bool isDone();

private:
    void clientLogic(int sockDesc);
    void getPorts(int sockDesc, int & tcpDownloadServerPort, int & tcpUploadServerPort, int & udpDownloadServerPort, int & udpUploadServerPort, int & tcpDownloadStreamServerPort, int & tcpUploadStreamServerPort, int & udpDownloadStreamServerPort, int & udpUploadStreamServerPort);
};

#endif