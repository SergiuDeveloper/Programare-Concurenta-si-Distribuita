#ifndef UDP_UPLOAD_TRANSMISSION
#define UDP_UPLOAD_TRANSMISSION

#include <iostream>
#include <unordered_map>

#include "../../../base/UDPServer/UDPServer.h"
#include "../../control/TimestampsHandler/TimestampsHandler.h"


class UDPUploadTransmission : public UDPServer {
private:
    TimestampsHandler * timestampsHandler;
    int chunkSize;
    bool acknowledge;
    std::unordered_map<char *, int> byteCounter;
    std::mutex * byteCounterMutex;

public:
    UDPUploadTransmission(TimestampsHandler * timestampsHandler, int port, int chunkSize, bool acknowledge);
    ~UDPUploadTransmission();

private:
    void serverLogic();
    void setByteCounterValue(char * clientIP, int chunkIndex);
};

#endif