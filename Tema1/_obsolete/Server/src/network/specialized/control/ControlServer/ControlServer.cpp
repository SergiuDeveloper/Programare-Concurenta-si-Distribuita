#include "ControlServer.h"


ControlServer::ControlServer(int port, int tcpDownloadServerPort, int tcpUploadServerPort, int udpDownloadServerPort, int udpUploadServerPort, int tcpDownloadStreamServerPort, int tcpUploadStreamServerPort, int udpDownloadStreamServerPort, int udpUploadStreamServerPort, std::string benchmarkFilePath, int chunkSize) : TCPServer(port), TimestampsHandler() {
    this->tcpDownloadServer = new TCPDownloadTransmission(this, tcpDownloadServerPort, benchmarkFilePath, chunkSize, true);
    this->tcpDownloadStreamServer = new TCPDownloadTransmission(this, tcpDownloadStreamServerPort, benchmarkFilePath, chunkSize, false);
    this->tcpUploadServer = new TCPUploadTransmission(this, tcpUploadServerPort, chunkSize, true);
    this->tcpUploadStreamServer = new TCPUploadTransmission(this, tcpUploadStreamServerPort, chunkSize, false);
    this->udpDownloadServer = new UDPDownloadTransmission(this, udpDownloadServerPort, benchmarkFilePath, chunkSize, true);
    this->udpDownloadStreamServer = new UDPDownloadTransmission(this, udpDownloadStreamServerPort, benchmarkFilePath, chunkSize, false);
    this->udpUploadServer = new UDPUploadTransmission(this, udpUploadServerPort, chunkSize, true);
    this->udpUploadStreamServer = new UDPUploadTransmission(this, udpUploadStreamServerPort, chunkSize, false);
    
    this->tcpDownloadServerPort = tcpDownloadServerPort;
    this->tcpUploadServerPort = tcpUploadServerPort;
    this->udpDownloadServerPort = udpDownloadServerPort;
    this->udpUploadServerPort = udpUploadServerPort;
    this->tcpDownloadStreamServerPort = tcpDownloadStreamServerPort;
    this->tcpUploadStreamServerPort = tcpUploadStreamServerPort;
    this->udpDownloadStreamServerPort = udpDownloadStreamServerPort;
    this->udpUploadStreamServerPort = udpUploadStreamServerPort;
    
    this->activeClientsMutex = new std::mutex();
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

    delete this->activeClientsMutex;
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
    if (activeClients.find(clientIP) != activeClients.end()) {
        std::cerr<<"Client "<<clientIP<<" already connected\r\n";
        return;
    }

    activeClientsMutex->lock();
    activeClients.insert(clientIP);
    activeClientsMutex->unlock();

    sendPorts(clientSockDesc, clientIP);

    bool uploadOperation = false;
    bool success;
    for (int i = 0; i < BENCHMARKS_COUNT; i++) {
        success = satisfyRequest(clientSockDesc, clientIP, uploadOperation);
        if (!success) {
            return;
        }
        uploadOperation = !uploadOperation;
    }

    activeClientsMutex->lock();
    activeClients.erase(clientIP);
    activeClientsMutex->unlock();
}

void ControlServer::sendPorts(int clientSockDesc, char * clientIP) {
    send(clientSockDesc, &tcpDownloadServerPort, sizeof(int), 0);
    send(clientSockDesc, &tcpUploadServerPort, sizeof(int), 0);
    send(clientSockDesc, &udpDownloadServerPort, sizeof(int), 0);
    send(clientSockDesc, &udpUploadServerPort, sizeof(int), 0);
    send(clientSockDesc, &tcpDownloadStreamServerPort, sizeof(int), 0);
    send(clientSockDesc, &tcpUploadStreamServerPort, sizeof(int), 0);
    send(clientSockDesc, &udpDownloadStreamServerPort, sizeof(int), 0);
    send(clientSockDesc, &udpUploadStreamServerPort, sizeof(int), 0);
}

bool ControlServer::satisfyRequest(int clientSockDesc, char * clientIP, bool uploadOperation) {
    uint8_t * readBuffer = new uint8_t[1];
    int readBytes;
    while ((readBytes = read(clientSockDesc, readBuffer, 1)) < 0) {
        std::cerr<<"Failed to read from socket\r\n";
    }
    if (readBytes == 0) {
        activeClientsMutex->lock();
        activeClients.erase(clientIP);
        activeClientsMutex->unlock();
        std::cerr<<"Client disconnected\r\n";
        return false;
    }

    time_t timestamp = getTimestamp(clientIP);
    int bytesCount = getBytesCount(clientIP);

    std::cout<<"Satisfied request: timestamp "<<timestamp<<", bytes count "<<bytesCount<<"\r\n";

    send(clientSockDesc, &timestamp, sizeof(time_t), 0);
    send(clientSockDesc, &bytesCount, sizeof(int), 0);

    return true;
}