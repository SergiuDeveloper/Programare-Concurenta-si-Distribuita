#define CONTROL_SERVER_PORT 8000
#define TCP_DOWNLOAD_SERVER_PORT 8001
#define TCP_UPLOAD_SERVER_PORT 8002
#define UDP_DOWNLOAD_SERVER_PORT 8003
#define UDP_UPLOAD_SERVER_PORT 8004
#define TCP_DOWNLOAD_STREAM_SERVER_PORT 8005
#define TCP_UPLOAD_STREAM_SERVER_PORT 8006
#define UDP_DOWNLOAD_STREAM_SERVER_PORT 8007
#define UDP_UPLOAD_STREAM_SERVER_PORT 8008

#define BENCHMARK_FILE_PATH "../data/test.mp4"
#define CHUNK_SIZE 65535

#include "network/specialized/control/ControlServer/ControlServer.h"


int main(int argc, char ** argv) {
    ControlServer controlServer(CONTROL_SERVER_PORT, TCP_DOWNLOAD_SERVER_PORT, TCP_UPLOAD_SERVER_PORT, UDP_DOWNLOAD_SERVER_PORT, UDP_UPLOAD_SERVER_PORT, TCP_DOWNLOAD_STREAM_SERVER_PORT, TCP_UPLOAD_STREAM_SERVER_PORT, UDP_DOWNLOAD_STREAM_SERVER_PORT, UDP_UPLOAD_STREAM_SERVER_PORT, BENCHMARK_FILE_PATH, CHUNK_SIZE);
    controlServer.run();
    
    while (true) {
    }

    return 0;
}