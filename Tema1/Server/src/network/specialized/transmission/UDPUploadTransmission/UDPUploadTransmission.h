#ifndef UDP_UPLOAD_TRANSMISSION
#define UDP_UPLOAD_TRANSMISSION

#define UDP_MAX_BUFFER_SIZE 65535

#include <iostream>
#include <unordered_map>

#include "../../../base/UDPServer/UDPServer.h"


class UDPUploadTransmission : private UDPServer {
private:
    int chunkSize;
    bool acknowledge;
    std::unordered_map<char *, int> byteCounter;
    std::mutex * byteCounterMutex;

public:
    UDPUploadTransmission(int port, int chunkSize, bool acknowledge);
    ~UDPUploadTransmission();

private:
    void serverLogic();
    void setByteCounterValue(char * clientIP, int chunkIndex);
};

#endif