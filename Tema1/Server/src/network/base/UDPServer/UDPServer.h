#ifndef UDP_SERVER
#define UDP_SERVER

#include <iostream>
#include <thread>
#include <mutex>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>


class UDPServer {
private:
    int port;
    bool running;
    std::mutex * runningMutex;
    int sockDesc;
    std::mutex * sockDescMutex;

public:
    UDPServer(int port);
    ~UDPServer();
    void run();
    void stop();

protected:
    virtual void serverLogic() = 0;
    bool getRunning();
    void setRunning(bool running);
    int getSockDesc();

private:
    void serverDaemon(int sockDesc);
};

#endif