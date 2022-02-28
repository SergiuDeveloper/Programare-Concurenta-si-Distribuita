mkdir -p build && \

g++ \
src/network/base/TCPClient/TCPClient.cpp \
src/network/base/UDPClient/UDPClient.cpp \
src/network/specialized/transmission/TCPDownloadTransmissionClient/TCPDownloadTransmissionClient.cpp \
src/main.cpp \
-o build/Client.out