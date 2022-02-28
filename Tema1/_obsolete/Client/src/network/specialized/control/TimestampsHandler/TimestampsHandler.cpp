#include "TimestampsHandler.h"


time_t TimestampsHandler::getTimestamp() {
    return timestamp;
}

void TimestampsHandler::setTimestamp(time_t timestamp) {
    this->timestamp = timestamp;
}

int TimestampsHandler::getBytesCount() {
    return bytesCount;
}

void TimestampsHandler::setBytesCount(int bytesCount) {
    this->bytesCount = bytesCount;
}