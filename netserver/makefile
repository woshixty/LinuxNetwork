CXX = g++
CXXFLAGS = -g -std=c++11
LDFLAGS = -lpthread

# 源文件
CLIENT_SRC = client.cpp
SERVER_SRC = echoserver.cpp InetAddress.cpp Socket.cpp Epoll.cpp Channel.cpp \
             EventLoop.cpp TcpServer.cpp Acceptor.cpp Connection.cpp Buffer.cpp \
             EchoServer.cpp ThreadPool.cpp Timestamp.cpp

# 目标文件
CLIENT_OBJ = $(CLIENT_SRC:.cpp=.o)
SERVER_OBJ = $(SERVER_SRC:.cpp=.o)

# 可执行文件
TARGETS = client echoserver

all: $(TARGETS)

client: $(CLIENT_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

echoserver: $(SERVER_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(TARGETS) $(CLIENT_OBJ) $(SERVER_OBJ)

.PHONY: all clean
