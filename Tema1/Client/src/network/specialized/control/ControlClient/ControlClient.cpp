#include "ControlClient.h"


ControlClient::ControlClient(char * ip, int port, int chunkSize, std::string benchmarkFilePath) : TCPClient(ip, port), TimestampsHandler() {
    if (chunkSize > TCP_MAX_BUFFER_SIZE || chunkSize <= 0) {
        throw new std::logic_error("Invalid chunk size (1 <= CHUNK_SIZE <= 65535)");
    }
    
    this->chunkSize = chunkSize;
    this->benchmarkFilePath = benchmarkFilePath;
    this->done = false;
}

void ControlClient::clientLogic(int sockDesc) {
    getPorts(sockDesc, tcpDownloadServerPort, tcpUploadServerPort, udpDownloadServerPort, udpUploadServerPort, tcpDownloadStreamServerPort, tcpUploadStreamServerPort, udpDownloadStreamServerPort, udpUploadStreamServerPort);

    TCPDownloadTransmissionClient * tcpDownloadTransmissionClient = new TCPDownloadTransmissionClient(this, ip, tcpDownloadServerPort, chunkSize, benchmarkFilePath, true);
    tcpDownloadTransmissionClient->run();
}

void ControlClient::tcpDownloadDone() {
    usleep(1000000);

    send(sockDesc, "1", 1, 0);

    usleep(1000000);

    time_t startTimestamp;
    int expectedBytesCount = read(sockDesc, &startTimestamp, sizeof(time_t));
    if (expectedBytesCount <= 0) {
        throw new std::logic_error("Failed to retrieve data from server");
    }

    time_t endTimestamp = getTimestamp();
    int bytesCount = getBytesCount();

    std::cout<<startTimestamp<<"\n"<<endTimestamp<<"\n"<<expectedBytesCount<<"\n"<<bytesCount<<"\n\n";

    TCPUploadTransmissionClient * tcpUploadTransmissionClient = new TCPUploadTransmissionClient(this, ip, tcpDownloadServerPort, chunkSize, benchmarkFilePath, true);
    tcpUploadTransmissionClient->run();
}

void ControlClient::tcpUploadDone() {
    usleep(1000000);

    send(sockDesc, "1", 1, 0);

    usleep(1000000);

    time_t startTimestamp = getTimestamp();
    int expectedBytesCount = getBytesCount();

    time_t endTimestamp;
    int bytesCount = read(sockDesc, &endTimestamp, sizeof(time_t));
    if (bytesCount <= 0) {
        throw new std::logic_error("Failed to retrieve data from server");
    }

    std::cout<<startTimestamp<<"\n"<<endTimestamp<<"\n"<<expectedBytesCount<<"\n"<<bytesCount<<"\n\n";

    UDPDownloadTransmissionClient * udpDownloadTransmissionClient = new UDPDownloadTransmissionClient(this, ip, tcpDownloadServerPort, chunkSize, benchmarkFilePath, true);
    udpDownloadTransmissionClient->run();
}

void ControlClient::udpDownloadDone() {
    usleep(1000000);

    send(sockDesc, "1", 1, 0);

    time_t startTimestamp;
    int expectedBytesCount = read(sockDesc, &startTimestamp, sizeof(time_t));
    if (expectedBytesCount <= 0) {
        throw new std::logic_error("Failed to retrieve data from server");
    }

    time_t endTimestamp = getTimestamp();
    int bytesCount = getBytesCount();

    std::cout<<startTimestamp<<"\n"<<endTimestamp<<"\n"<<expectedBytesCount<<"\n"<<bytesCount<<"\n\n";

    UDPUploadTransmissionClient * udpDownloadTransmissionClient = new UDPUploadTransmissionClient(this, ip, tcpDownloadServerPort, chunkSize, benchmarkFilePath, true);
    udpDownloadTransmissionClient->run();
}

void ControlClient::udpUploadDone() {
    send(sockDesc, "1", 1, 0);

    time_t startTimestamp = getTimestamp();
    int expectedBytesCount = getBytesCount();

    time_t endTimestamp;
    int bytesCount = read(sockDesc, &endTimestamp, sizeof(time_t));
    if (bytesCount <= 0) {
        throw new std::logic_error("Failed to retrieve data from server");
    }

    TCPDownloadTransmissionClient * tcpDownloadTransmissionClient = new TCPDownloadTransmissionClient(this, ip, tcpDownloadServerPort, chunkSize, benchmarkFilePath, false);
    tcpDownloadTransmissionClient->run();
}

void ControlClient::tcpDownloadStreamDone() {
    send(sockDesc, "1", 1, 0);

    time_t startTimestamp;
    int expectedBytesCount = read(sockDesc, &startTimestamp, sizeof(time_t));
    if (expectedBytesCount <= 0) {
        throw new std::logic_error("Failed to retrieve data from server");
    }

    time_t endTimestamp = getTimestamp();
    int bytesCount = getBytesCount();

    TCPUploadTransmissionClient * tcpUploadTransmissionClient = new TCPUploadTransmissionClient(this, ip, tcpDownloadServerPort, chunkSize, benchmarkFilePath, false);
    tcpUploadTransmissionClient->run();
}

void ControlClient::tcpUploadStreamDone() {
    send(sockDesc, "1", 1, 0);

    time_t startTimestamp = getTimestamp();
    int expectedBytesCount = getBytesCount();

    time_t endTimestamp;
    int bytesCount = read(sockDesc, &endTimestamp, sizeof(time_t));
    if (bytesCount <= 0) {
        throw new std::logic_error("Failed to retrieve data from server");
    }

    UDPDownloadTransmissionClient * udpDownloadTransmissionClient = new UDPDownloadTransmissionClient(this, ip, tcpDownloadServerPort, chunkSize, benchmarkFilePath, false);
    udpDownloadTransmissionClient->run();
}

void ControlClient::udpDownloadStreamDone() {
    send(sockDesc, "1", 1, 0);

    time_t startTimestamp;
    int expectedBytesCount = read(sockDesc, &startTimestamp, sizeof(time_t));
    if (expectedBytesCount <= 0) {
        throw new std::logic_error("Failed to retrieve data from server");
    }

    time_t endTimestamp = getTimestamp();
    int bytesCount = getBytesCount();

    UDPUploadTransmissionClient * udpDownloadTransmissionClient = new UDPUploadTransmissionClient(this, ip, tcpDownloadServerPort, chunkSize, benchmarkFilePath, false);
    udpDownloadTransmissionClient->run();
}

void ControlClient::udpUploadStreamDone() {
    send(sockDesc, "1", 1, 0);

    time_t startTimestamp = getTimestamp();
    int expectedBytesCount = getBytesCount();

    time_t endTimestamp;
    int bytesCount = read(sockDesc, &endTimestamp, sizeof(time_t));
    if (bytesCount <= 0) {
        throw new std::logic_error("Failed to retrieve data from server");
    }

    std::cout<<"Benchmark done\r\n";

    done = true;
}

bool ControlClient::isDone() {
    return done;
}

void ControlClient::getPorts(int sockDesc, int & tcpDownloadServerPort, int & tcpUploadServerPort, int & udpDownloadServerPort, int & udpUploadServerPort, int & tcpDownloadStreamServerPort, int & tcpUploadStreamServerPort, int & udpDownloadStreamServerPort, int & udpUploadStreamServerPort) {
    int readBytes = read(sockDesc, &tcpDownloadServerPort, sizeof(int));
    if (readBytes <= 0) {
        throw new std::logic_error("Failed to communicate with server");
    }
    readBytes = read(sockDesc, &tcpUploadServerPort, sizeof(int));
    if (readBytes <= 0) {
        throw new std::logic_error("Failed to communicate with server");
    }
    readBytes = read(sockDesc, &udpDownloadServerPort, sizeof(int));
    if (readBytes <= 0) {
        throw new std::logic_error("Failed to communicate with server");
    }
    readBytes = read(sockDesc, &udpUploadServerPort, sizeof(int));
    if (readBytes <= 0) {
        throw new std::logic_error("Failed to communicate with server");
    }
    readBytes = read(sockDesc, &tcpDownloadStreamServerPort, sizeof(int));
    if (readBytes <= 0) {
        throw new std::logic_error("Failed to communicate with server");
    }
    readBytes = read(sockDesc, &tcpUploadStreamServerPort, sizeof(int));
    if (readBytes <= 0) {
        throw new std::logic_error("Failed to communicate with server");
    }
    readBytes = read(sockDesc, &udpDownloadStreamServerPort, sizeof(int));
    if (readBytes <= 0) {
        throw new std::logic_error("Failed to communicate with server");
    }
    readBytes = read(sockDesc, &udpUploadStreamServerPort, sizeof(int));
    if (readBytes <= 0) {
        throw new std::logic_error("Failed to communicate with server");
    }
}