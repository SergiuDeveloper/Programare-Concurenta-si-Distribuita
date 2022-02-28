#include "TCPServer.h"


TCPServer::TCPServer(int port) {
    this->port = port;
    this->running = false;
    this->runningMutex = new std::mutex();
}

TCPServer::~TCPServer() {
    delete this->runningMutex;
}

void TCPServer::run() {
    if (getRunning()) {
        throw std::logic_error("Server already running");
    }
    setRunning(true);

    int sockDesc = socket(AF_INET, SOCK_STREAM, 0);
    if (sockDesc < 0) {
        throw std::runtime_error("Failed to open socket");
    }

    struct sockaddr_in sockAddr = {};
    sockAddr.sin_family = AF_INET;
    sockAddr.sin_addr.s_addr = INADDR_ANY;
    sockAddr.sin_port = htons(port);
    int bindRes = bind(sockDesc, (struct sockaddr *)&sockAddr, sizeof(sockAddr));
    if (bindRes < 0) {
        throw std::runtime_error("Failed to bind socket");
    }

    std::thread serverDaemonThread(&TCPServer::serverDaemon, this, sockDesc);
    serverDaemonThread.detach();
}

void TCPServer::stop() {
    if (!getRunning()) {
        throw std::logic_error("Server not running");
    }
    setRunning(false);
}

void TCPServer::serverDaemon(int sockDesc) {
    std::cout<<"Listening for connections\r\n";
    listen(sockDesc, SOMAXCONN);

    while (getRunning()) {
        struct sockaddr_in clientSockAddr;
        socklen_t clientSockAddrSize = sizeof(clientSockAddr);
        int clientSockDesc = accept(sockDesc, (struct sockaddr *)&clientSockAddr, &clientSockAddrSize);
        if (clientSockDesc < 0) {
            std::cerr<<"Failed to accept client connection\r\n";
            continue;
        }
        char * clientIP = inet_ntoa(clientSockAddr.sin_addr);
        std::cout<<"Accepted client connection from "<<clientIP<<"\r\n";

        std::thread handleClientThread(&TCPServer::handleClientEncaps, this, clientSockDesc, clientIP);
        handleClientThread.detach();
    }

    close(sockDesc);
}

void TCPServer::handleClientEncaps(int clientSockDesc, char * clientIP) {
    handleClient(clientSockDesc, clientIP);

    close(clientSockDesc);
}

bool TCPServer::getRunning() {
    runningMutex->lock();
    bool runningVal = running;
    runningMutex->unlock();

    return runningVal;
}

void TCPServer::setRunning(bool running) {
    runningMutex->lock();
    this->running = running;
    runningMutex->unlock();
}