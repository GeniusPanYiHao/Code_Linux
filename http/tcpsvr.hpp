#include<stdio.h>
#include<iostream>
#include<unistd.h>
#include<string>
#include<string.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>

class TcpSvr
{
  public:
    TcpSvr()
    {
        Sock=-1;
    }
    ~TcpSvr()
    {

    }
    bool Createsock()
    {
      Sock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
      if(Sock<0)
      {
        perror("socket");
        return false;
      }
      return true;
    }

    bool Bind(std::string& ip,uint16_t port)
    {
      struct sockaddr_in addr;
      addr.sin_family=AF_INET;
      addr.sin_port=htons(port);
      addr.sin_addr.s_addr=inet_addr(ip.c_str());
      int ret=bind(Sock,(struct sockaddr*)&addr,sizeof(addr));
      if(ret<0)
      {
        perror("bind");
        return false;
      }
      return true;


    }
    
    bool Listen(int Backlog=5)
    {
     int ret= listen(Sock,Backlog);
     if(ret<0)
     {
        perror("listen");
       return false;
     }
     return true;

    }

    bool Connect(std::string& ip,uint16_t port)//:连接接口，对客户端而言，他需要知道要连接的IP地址和端口信息
    { 
      struct sockaddr_in destaddr;
      destaddr.sin_family=AF_INET;
      destaddr.sin_port=htons(port);
      destaddr.sin_addr.s_addr=inet_addr(ip.c_str());
      int ret=connect(Sock,(struct sockaddr*)&destaddr,sizeof(destaddr));
      if(ret<0)
      {
        perror("connect");
        return false;
      }
      return true;
        

    }
    bool Accept(TcpSvr& ts,struct sockaddr_in* addr=NULL)
    {
      struct sockaddr_in peeraddr;
      socklen_t addrlen=sizeof(struct sockaddr_in);
      int newsock=accept(Sock,(struct sockaddr*)&peeraddr,&addrlen);
      if(newsock<0)
      {
        return false;
      }
      ts.Sock=newsock;
      if(addr!=NULL)
      {
        memcpy(addr,&peeraddr,addrlen);
      }
      return true;

    }
    bool Recv(std::string& buffer)
    { 
        char buf[1024]={0};
        int ret=recv(Sock,buf,sizeof(buf)-1,0);
        if(ret<0)
        {
            perror("recv failed");
            return false;
        }
        else if(ret==0)
        {
          //:对端连接将关闭
          printf("peer close this connect");
          return false;
        }
        buffer.assign(buf,ret);
        return true;
    }

    bool Send(std::string buffer)
    {
        int ret=send(Sock,buffer.c_str(),buffer.size(),0);
        if(ret<0)
        {
          perror("send");
          return false;
        }
        return 0;
    }
    
    void Close()
    {
        close(Sock);
        Sock=-1;
    }
  private:
    int Sock;
  
};
