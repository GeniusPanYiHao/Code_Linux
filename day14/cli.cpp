#include"tcpsvr.hpp"

int main(int argc,char* argv[])
{
      if(argc!=3)
      {
        printf("./cli[ip][port]\n");
        return 0;
      }

      std::string ip=argv[1];
      uint16_t port=atoi(argv[2]);


      TcpSvr ts;
      if(!ts.Createsock())
      {
        return 0;
      }
      if(!ts.Connect(ip,port))
      {
        return 0;
      }
      while(1)
      {
        printf("client say:");
        fflush(stdout);
        std::string buf;
        std::cin>>buf;
        ts.Send(buf);
        ts.Recv(buf);
        printf("server say:%s\n",buf.c_str());
      }
      ts.Close();
      return 0;
}
