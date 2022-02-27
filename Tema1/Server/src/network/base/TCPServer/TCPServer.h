#ifndef TCP_SERVER
#define TCP_SERVER

#include <iostream>
#include <thread>
#include <mutex>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>


class TCPServer {
private:
    int port;
    bool running;
    std::mutex * runningMutex;

public:
    TCPServer(int port);
    ~TCPServer();
    void run();
    void stop();

protected:
    virtual void handleClient(int clientSockDesc, char * clientIP) = 0;
    bool getRunning();
    void setRunning(bool running);

private:
    void serverDaemon(int sockDesc);
    void handleClientEncaps(int clientSockDesc, char * clientIP);
};

#endif