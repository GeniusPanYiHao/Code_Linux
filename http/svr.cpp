#include"tcpsvr.hpp"
#include<string.h>
#include<sstream>
int main(int argc,char* argv[])
{
  if(argc!=3)
  {
    printf("./svr[ip][port]\n");
    return 0;
  }
  std::string ip=argv[1];
  uint16_t port=atoi(argv[2]);
  TcpSvr ts;
  if(!ts.Createsock())
  {
      return 0;
  }
  if(!ts.Bind(ip,port))
  {
    return 0;
  }
  if(!ts.Listen())
  {
    return 0;
  }
  TcpSvr peerts;
  struct sockaddr_in peeraddr;
  while(1)
  {
    if(!ts.Accept(peerts,&peeraddr));
    {
        return 0;
    }
    printf("svr have a new connect, ip:port --> %s:%d\n", 
    inet_ntoa(peeraddr.sin_addr), ntohs(peeraddr.sin_port));
    std::string buf;
    peerts.Recv(buf);
    printf("liulanqi say:%s\n",buf.c_str());
    //:正文信息
    std::string body="<html><h1>LINUX</h1/></html>";
    //：响应首行
    //：响应头部
    //Content-Length: \r\n
    //Content-Type:-r-n
    //:\r\n
    //:正文
    char buffer[10240]={0};
    snprintf(buffer,sizeof(buffer)-1,"%s %s %s\r\nContent-Length: %lu\r\nContent-Type: %s\r\n\r\n","HTTP/1.1","200","OK",body.size(),"text/html");
    std::string header;
    header.assign(buffer,strlen(buffer));
    peerts.Send(header);
    peerts.Send(body);
    peerts.Close();
  }
  ts.Close();
  return 0;
}
