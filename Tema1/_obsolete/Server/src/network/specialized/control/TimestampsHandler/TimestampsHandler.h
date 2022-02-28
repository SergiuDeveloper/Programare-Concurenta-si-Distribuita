#ifndef TIMESTAMPS_HANDLER
#define TIMESTAMPS_HANDLER

#include <unordered_map>
#include <ctime>
#include <mutex>


class TimestampsHandler {
private:
    std::unordered_map<char *, time_t> timestamps;
    std::mutex * timestampsMutex;
    std::unordered_map<char *, int> bytesCountMap;
    std::mutex * bytesCountMutex;

public:
    TimestampsHandler();
    ~TimestampsHandler();   

public:
    void setTimestamp(char * clientIP, time_t timestamp);
    time_t getTimestamp(char * clientIP);
    void setBytesCount(char * clientIP, int bytesCount);
    int getBytesCount(char * clientIP);
};

#endif