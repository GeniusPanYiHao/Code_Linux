#include"Udpsvr.hpp"

int main(int argc,char* argv[])
{


  if(argc!=3)
  {
      printf("./svr[ip][port]\n");
      return 0;
  }
  std::string ip=argv[1];
  uint16_t port= atoi(argv[2]);

  udpsvr us;
  if(!us.createSock())
  {
    return 0;
  }
  if(!us.Bind(ip,port))
  {
    return 0;
  }
  while(1)//:接收数据，发送数据
  {
      std::string buf;
      struct sockaddr_in peeraddr;
      us.Recv(buf,&peeraddr);

      printf("client say:%s\n",buf.c_str());

      printf("server say:");

      fflush(stdout);

      std::cin>>buf;

      us.Send(buf,&peeraddr);

  }
  us.Close();
}
