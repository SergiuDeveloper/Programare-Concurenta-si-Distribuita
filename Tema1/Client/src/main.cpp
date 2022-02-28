#define IP (char *)"192.168.0.1"
#define PORT 8000

#define BENCHMARK_FILE_PATH "../data/test.mp4"
#define CHUNK_SIZE 65535

#include "network/specialized/control/ControlClient/ControlClient.h"


int main(int argc, char ** argv) {
    ControlClient controlClient(IP, PORT, CHUNK_SIZE, BENCHMARK_FILE_PATH);
    controlClient.run();
    
    while (!controlClient.isDone()) {
    }

    return 0;
}