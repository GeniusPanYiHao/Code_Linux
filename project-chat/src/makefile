CFLAGS=-std=c++11 -g
LDFLAGS=-lpthread -ljsoncpp

#SVRBIN=../bin/svr/ChatSvr
SVRBIN=./ChatSvr
CLIBIN=./Client

.PHONY:all
all:$(SVRBIN) $(CLIBIN)
$(SVRBIN):ChatSever.cpp
	g++ $^ -o $@ $(CFLAGS) $(LDFLAGS)
$(CLIBIN):ChatClient.cpp
	g++ $^ -o $@ $(CFLAGS) $(LDFLAGS) -lncurses

.PHONY:clean
clean:
	rm -f $(SVRBIN) $(CLIBIN)
