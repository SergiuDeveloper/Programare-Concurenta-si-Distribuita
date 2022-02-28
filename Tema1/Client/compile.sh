mkdir -p build && \

g++ \
src/network/base/TCPClient/TCPClient.cpp \
src/network/base/UDPClient/UDPClient.cpp \
src/network/specialized/transmission/TCPDownloadTransmissionClient/TCPDownloadTransmissionClient.cpp \
src/network/specialized/transmission/TCPUploadTransmissionClient/TCPUploadTransmissionClient.cpp \
src/network/specialized/transmission/UDPDownloadTransmissionClient/UDPDownloadTransmissionClient.cpp \
src/network/specialized/transmission/UDPUploadTransmissionClient/UDPUploadTransmissionClient.cpp \
src/network/specialized/control/TimestampsHandler/TimestampsHandler.cpp \
src/network/specialized/control/ControlClient/ControlClient.cpp \
src/main.cpp \
-o build/Client.out