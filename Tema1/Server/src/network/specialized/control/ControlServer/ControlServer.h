#ifndef CONTROL_SERVER
#define CONTROL_SERVER

#include "../../../base/TCPServer/TCPServer.h"


class ControlServer : private TCPServer {
public:
    ControlServer(int port);
};

#endif