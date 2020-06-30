CFLAGS=-std=c++11 -g
LDFLAGS=-lpthread -ljsoncpp
.PHONY:ChatSvr
ChatSvr:ChatSever.cpp
	g++ $^ -o $@ $(CFLAGS) $(LDFLAGS)
