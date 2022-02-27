#include "ControlServer.h"


ControlServer::ControlServer(int port, int tcpDownloadServerPort, int tcpUploadServerPort, int udpDownloadServerPort, int udpUploadServerPort, int tcpDownloadStreamServerPort, int tcpUploadStreamServerPort, int udpDownloadStreamServerPort, int udpUploadStreamServerPort, std::string benchmarkFilePath, int chunkSize) : TCPServer(port) {
    this->tcpDownloadServer = new TCPDownloadTransmission(tcpDownloadServerPort, benchmarkFilePath, chunkSize, true);
    this->tcpDownloadStreamServer = new TCPDownloadTransmission(tcpDownloadServerPort, benchmarkFilePath, chunkSize, false);
    this->tcpUploadServer = new TCPUploadTransmission(tcpDownloadServerPort, chunkSize, true);
    this->tcpUploadStreamServer = new TCPUploadTransmission(tcpDownloadServerPort, chunkSize, false);
    this->udpDownloadServer = new UDPDownloadTransmission(tcpDownloadServerPort, benchmarkFilePath, chunkSize, true);
    this->udpDownloadStreamServer = new UDPDownloadTransmission(tcpDownloadServerPort, benchmarkFilePath, chunkSize, false);
    this->udpUploadServer = new UDPUploadTransmission(tcpDownloadServerPort, chunkSize, true);
    this->udpUploadStreamServer = new UDPUploadTransmission(tcpDownloadServerPort, chunkSize, false);
    
    this->tcpDownloadServerPort = tcpDownloadServerPort;
    this->tcpUploadServerPort = tcpUploadServerPort;
    this->udpDownloadServerPort = udpDownloadServerPort;
    this->udpUploadServerPort = udpUploadServerPort;
    this->tcpDownloadStreamServerPort = tcpDownloadStreamServerPort;
    this->tcpUploadStreamServerPort = tcpUploadStreamServerPort;
    this->udpDownloadStreamServerPort = udpDownloadStreamServerPort;
    this->udpUploadStreamServerPort = udpUploadStreamServerPort;
}

ControlServer::~ControlServer() {
    delete this->tcpDownloadServer;
    delete this->tcpUploadServer;
    delete this->udpDownloadServer;
    delete this->udpUploadServer;
    delete this->tcpDownloadStreamServer;
    delete this->tcpUploadStreamServer;
    delete this->udpDownloadStreamServer;
    delete this->udpUploadStreamServer;
}

void ControlServer::run() {
    this->tcpDownloadServer->run();
    this->tcpUploadServer->run();
    this->udpDownloadServer->run();
    this->udpUploadServer->run();
    this->tcpDownloadStreamServer->run();
    this->tcpUploadStreamServer->run();
    this->udpDownloadStreamServer->run();
    this->udpUploadStreamServer->run();

    TCPServer::run();
}

void ControlServer::stop() {
    this->tcpDownloadServer->stop();
    this->tcpUploadServer->stop();
    this->udpDownloadServer->stop();
    this->udpUploadServer->stop();
    this->tcpDownloadStreamServer->stop();
    this->tcpUploadStreamServer->stop();
    this->udpDownloadStreamServer->stop();
    this->udpUploadStreamServer->stop();

    TCPServer::stop();
}

void ControlServer::handleClient(int clientSockDesc, char * clientIP) {
    if (activeClients.find(clientIP) == activeClients.end()) {
        return;
    }

    send(clientSockDesc, &tcpDownloadServerPort, sizeof(int), 0);
    send(clientSockDesc, &tcpUploadServerPort, sizeof(int), 0);
    send(clientSockDesc, &udpDownloadServerPort, sizeof(int), 0);
    send(clientSockDesc, &udpUploadServerPort, sizeof(int), 0);
    send(clientSockDesc, &tcpDownloadStreamServerPort, sizeof(int), 0);
    send(clientSockDesc, &tcpUploadStreamServerPort, sizeof(int), 0);
    send(clientSockDesc, &udpDownloadStreamServerPort, sizeof(int), 0);
    send(clientSockDesc, &udpUploadStreamServerPort, sizeof(int), 0);

    activeClients.erase(clientIP);
}