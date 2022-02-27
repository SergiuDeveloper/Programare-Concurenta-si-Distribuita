#ifndef TCP_UPLOAD_TRANSMISSION
#define TCP_UPLOAD_TRANSMISSION

#include "../../../base/TCPServer/TCPServer.h"


class TCPUploadTransmission : private TCPServer {
private:
    bool acknowledge;

public:
    TCPUploadTransmission(int port, bool acknowledge);

private:
    void handleClient(int clientSockDesc, char * clientIP);
    void receiveBenchmarkFile(int clientSockDesc, char * clientIP);
};

#endif