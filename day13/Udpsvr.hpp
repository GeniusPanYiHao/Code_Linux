#include<iostream>
#include<stdio.h>
#include<unistd.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<string>
#include<stdlib.h>
#include<arpa/inet.h>
class udpsvr
{
  public:
    udpsvr()
    {
      Sock=-1;
    }
    ~udpsvr()
    {

    }
    bool createSock()//:创建套接字
    {
      Sock=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
      if(Sock<0)
      {
        perror("socket");
        return false;
      }
      return true;

    }
    bool Bind(std::string &ip,uint16_t port)//:绑定
    {
      struct sockaddr_in addr;
      addr.sin_family=AF_INET;
      addr.sin_port=htons(port);
      addr.sin_addr.s_addr=inet_addr(ip.c_str());
     int ret= bind(Sock,(struct sockaddr*)&addr,sizeof(addr));
     if(ret<0)
     {
       perror("bind");
       return false;
     }
     return true;

    }

    bool Send(std::string &buf,struct sockaddr_in* destaddr)//:发送
    {
     int sendsize= sendto(Sock,buf.c_str(),buf.size(),0,(struct sockaddr*)destaddr,sizeof(struct sockaddr_in));
     if(sendsize<0)
     {
       perror("send");
       return false;
     }
     return true;
    }
    bool Recv(std::string& buf,struct sockaddr_in* srcaddr)
    {
      char tmp[1024]={0};
      socklen_t socklen=sizeof(struct sockaddr_in);
      int recvSize=recvfrom(Sock,tmp,sizeof(tmp)-1,0,(struct sockaddr*)srcaddr,&socklen);
      if(recvSize<0)
      {
        perror("recvfrom");
        return false;
      }
      buf.assign(tmp,recvSize);
      return true;
    }

    void Close()
    {
      close(Sock);
      Sock=-1;
    }
  private:
    int Sock;

};
