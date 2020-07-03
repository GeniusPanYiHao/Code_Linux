#include"tcpsvr.hpp"

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
  while(1)
  {
    TcpSvr peerts;
    struct sockaddr_in peeraddr;
    if(!ts.Accept(peerts,&peeraddr))
    {
        printf("svr have a new connect,ip,port->%s:%d\n",inet_ntoa(peeraddr.sin_addr),ntohs(peeraddr.sin_port));
    }
    std::string buf;
    peerts.Recv(buf);
    printf("client say:%s\n",buf.c_str());
    printf("sever say:");
    fflush(stdout);
    std::cin>>buf;
    peerts.Send(buf);
  }
  ts.Close();
  return 0;
}
