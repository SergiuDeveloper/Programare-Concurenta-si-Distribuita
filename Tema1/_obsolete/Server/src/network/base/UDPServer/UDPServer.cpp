#include "UDPServer.h"


UDPServer::UDPServer(int port) {
    this->port = port;
    this->running = false;
    this->runningMutex = new std::mutex();
    this->sockDescMutex = new std::mutex();
}

UDPServer::~UDPServer() {
    delete this->runningMutex;
    delete this->sockDescMutex;
}

void UDPServer::run() {
    if (getRunning()) {
        throw std::logic_error("Server already running");
    }
    setRunning(true);

    sockDesc = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockDesc < 0) {
        throw std::runtime_error("Failed to open socket");
    }

    struct sockaddr_in sockAddr = {};
    sockAddr.sin_family = AF_INET;
    sockAddr.sin_addr.s_addr = INADDR_ANY;
    sockAddr.sin_port = htons(port);
    int bindRes = bind(sockDesc, (struct sockaddr *) &sockAddr, sizeof(sockAddr));
    if (bindRes < 0) {
        throw std::runtime_error("Failed to bind socket");
    }

    std::thread serverDaemonThread(&UDPServer::serverDaemon, this, sockDesc);
    serverDaemonThread.detach();
}

void UDPServer::stop() {
    if (!getRunning()) {
        throw std::logic_error("Server not running");
    }
    setRunning(false);
}

void UDPServer::serverDaemon(int sockDesc) {
    serverLogic();

    close(sockDesc);
}

bool UDPServer::getRunning() {
    runningMutex->lock();
    bool runningVal = running;
    runningMutex->unlock();

    return runningVal;
}

void UDPServer::setRunning(bool running) {
    runningMutex->lock();
    this->running = running;
    runningMutex->unlock();
}

int UDPServer::getSockDesc() {
    sockDescMutex->lock();
    int sockDescVal = sockDesc;
    sockDescMutex->unlock();

    return sockDescVal;
}