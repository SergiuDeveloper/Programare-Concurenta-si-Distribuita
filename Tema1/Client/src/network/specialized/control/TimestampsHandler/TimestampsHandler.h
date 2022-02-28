#ifndef TIMESTAMPS_HANDLER
#define TIMESTAMPS_HANDLER

#include <ctime>


class TimestampsHandler {
private:
    time_t timestamp;
    int bytesCount;

public:
    virtual void tcpDownloadDone() = 0;
    virtual void tcpUploadDone() = 0;
    virtual void udpDownloadDone() = 0;
    virtual void udpUploadDone() = 0;
    virtual void tcpDownloadStreamDone() = 0;
    virtual void tcpUploadStreamDone() = 0;
    virtual void udpDownloadStreamDone() = 0;
    virtual void udpUploadStreamDone() = 0;

public:
    time_t getTimestamp();
    void setTimestamp(time_t timestamp);
    int getBytesCount();
    void setBytesCount(int bytesCount);
};

#endif