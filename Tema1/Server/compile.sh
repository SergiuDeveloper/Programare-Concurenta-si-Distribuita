g++ -pthread \
src/network/base/TCPServer/TCPServer.cpp \
src/network/base/UDPServer/UDPServer.cpp \
src/network/specialized/control/TimestampsHandler/TimestampsHandler.cpp \
src/network/specialized/transmission/TCPDownloadTransmission/TCPDownloadTransmission.cpp \
src/network/specialized/transmission/TCPUploadTransmission/TCPUploadTransmission.cpp \
src/network/specialized/transmission/UDPDownloadTransmission/UDPDownloadTransmission.cpp \
src/network/specialized/transmission/UDPUploadTransmission/UDPUploadTransmission.cpp \
src/network/specialized/control/ControlServer/ControlServer.cpp