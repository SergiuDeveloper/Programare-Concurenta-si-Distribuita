#ifndef TCP_UPLOAD_TRANSMISSION
#define TCP_UPLOAD_TRANSMISSION

#include "../../../base/TCPServer/TCPServer.h"
#include "../../control/TimestampsHandler/TimestampsHandler.h"


class TCPUploadTransmission : public TCPServer {
private:
    TimestampsHandler * timestampsHandler;
    int chunkSize;
    bool acknowledge;

public:
    TCPUploadTransmission(TimestampsHandler * timestampsHandler, int port, int chunkSize, bool acknowledge);

private:
    void handleClient(int clientSockDesc, char * clientIP);
    void receiveBenchmarkFile(int clientSockDesc, char * clientIP);
};

#endif