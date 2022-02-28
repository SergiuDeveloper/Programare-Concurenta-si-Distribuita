#ifndef UDP_CLIENT
#define UDP_CLIENT

#define UDP_MAX_BUFFER_SIZE 65535

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>


class UDPClient {
private:
    char * ip;
    int port;
    int sockDesc;

public:
    UDPClient(char * ip, int port);
    void run();

protected:
    virtual void clientLogic(int sockDesc) = 0;
};

#endif