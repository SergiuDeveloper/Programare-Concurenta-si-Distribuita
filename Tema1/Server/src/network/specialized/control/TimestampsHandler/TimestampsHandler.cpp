#include "TimestampsHandler.h"


TimestampsHandler::TimestampsHandler() {
    this->timestampsMutex = new std::mutex();
    this->bytesCountMutex = new std::mutex();
}

TimestampsHandler::~TimestampsHandler() {
    delete this->timestampsMutex;
    delete this->bytesCountMutex;
}

void TimestampsHandler::setTimestamp(char * clientIP, time_t timestamp) {
    timestampsMutex->lock();
    timestamps[clientIP] = timestamp;
    timestampsMutex->unlock();
}

time_t TimestampsHandler::getTimestamp(char * clientIP) {
    return timestamps[clientIP];
}

void TimestampsHandler::setBytesCount(char * clientIP, int bytesCount) {
    bytesCountMutex->lock();
    bytesCountMap[clientIP] = bytesCount;
    bytesCountMutex->unlock();
}

int TimestampsHandler::getBytesCount(char * clientIP) {
    return bytesCountMap[clientIP];
}