#ifndef TCP_UPLOAD_TRANSMISSION
#define TCP_UPLOAD_TRANSMISSION

#include "../../../base/TCPServer/TCPServer.h"


class TCPUploadTransmission : public TCPServer {
private:
    int chunkSize;
    bool acknowledge;

public:
    TCPUploadTransmission(int port, int chunkSize, bool acknowledge);

private:
    void handleClient(int clientSockDesc, char * clientIP);
    void receiveBenchmarkFile(int clientSockDesc, char * clientIP);
};

#endif