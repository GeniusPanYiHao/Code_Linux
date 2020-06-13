#include"Udpsvr.hpp"


int main(int argc,char* argv[])
{

  if(argc!=3)
  {
    printf("./cli[ip][port]\n");
    return 0;
  }

  std::string ip=argv[1];
  uint16_t port= atoi(argv[2]);

  udpsvr us;
  if(!us.createSock())
  {
    return 0;
  }
  struct sockaddr_in destaddr;
  destaddr.sin_family=AF_INET;
  destaddr.sin_addr.s_addr=inet_addr(ip.c_str());
  destaddr.sin_port=htons(port);


  while(1)//:发送数据，接收数据
  {
    std::string buf;
    printf("client say:");
    fflush(stdout);

    std::cin>>buf;

    us.Send(buf,&destaddr);

    us.Recv(buf,&destaddr);

    printf("server say:%s\n",buf.c_str());
  }
  us.Close();
  return 0;
}
