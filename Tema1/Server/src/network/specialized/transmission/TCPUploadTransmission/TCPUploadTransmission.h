#ifndef TCP_UPLOAD_TRANSMISSION
#define TCP_UPLOAD_TRANSMISSION

#define TCP_MAX_BUFFER_SIZE 65535

#include "../../../base/TCPServer/TCPServer.h"


class TCPUploadTransmission : private TCPServer {
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