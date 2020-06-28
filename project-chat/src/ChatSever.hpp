#pragma once 
#include<iostream>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<string>
#include<pthread.h>
#include<sys/types.h>
#include"MsgPool.hpp"
#include"ConnectInFo.hpp"
#include"LogSever.hpp"
#include"UserManger.hpp"
#define UDPPORT 17777
#define THREADCOUNT 2
#define TCPPORT 17779
class ChatSever
{
  public:
    ChatSever()
    {
        UdpSock_=-1;
        UdpPort_=UDPPORT;
        MsgPool_=NULL;
        TcpSock_=-1;
        TcpPort_=TCPPORT;
        UserMAnger=NULL;
    }
    ~ChatSever()
    {
      if(MsgPool_)
      {
        delete MsgPool_;
        MsgPool_=NULL;
      }
      if(UserMAnger)
      {
        delete UserMAnger;
        UserMAnger=NULL;
      }

    }
      //:上层调用InitSever函数来初始化UDP服务
    void InitSever()
    {
      //:创建UDP套接字
      UdpSock_=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
      if(UdpSock_<0)
      {
       LOG(FATAL,"create socket failed");
        exit(1);
      }
      //:绑定地址信息
      struct sockaddr_in addr;
      addr.sin_family=AF_INET;
      addr.sin_port=htons(UdpPort_);
      addr.sin_addr.s_addr=inet_addr("0.0.0.0");
      
      int ret=bind(UdpSock_,(struct sockaddr*)&addr,sizeof(addr));

      if(ret<0)
      {
        LOG(FATAL,"bind addr failed");
        exit(2);
      }
      LOG(INFO,"Udp bind success");
      //:初始化数据池
      MsgPool_=new MsgPool();
      if(!MsgPool_)
      {
        LOG(FATAL,"create MsgPool failed");
        exit(3);
      }
      LOG(INFO,"create MsgPool success");
      //:用户管理
      UserMAnger=new UserManger();
      if(!UserMAnger)
      { 
        LOG(FATAL,"create usermanger failed");
        exit(8);
      }
      //；创建TCP socket
      TcpSock_=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
      if(TcpSock_<0)
      {
        LOG(FATAL,"create TcpSock failed");
        exit(5);
      }
      struct sockaddr_in tcpaddr;
      tcpaddr.sin_family=AF_INET;
      tcpaddr.sin_port=htons(TcpPort_);
      tcpaddr.sin_addr.s_addr=inet_addr("0.0.0.0");
      ret=bind(TcpSock_,(struct sockaddr*)&tcpaddr,sizeof(tcpaddr));
      if(ret<0)
      {
        LOG(FATAL,"bind failed");
        exit(6);
      }
      //:监听
      ret=listen(TcpSock_,5);
      if(ret<0)
      {
        LOG(FATAL,"listen failed");
        exit(7);
      }
      LOG(INFO,"TCP listen 0.0.0.0:17779");

    }
    //:初始化程序中的生产者和消费者线程
    void Start()
    {
      pthread_t tid;
      for(int i=0;i<THREADCOUNT;i++)
      {
       int ret=pthread_create(&tid,NULL,ProductStart,(void*)this);
          if(ret<0)
          {
            LOG(FATAL,"pthread_create new thread failed");
            exit(4);
          }
          ret=pthread_create(&tid,NULL,ConsumeStart,(void*)this);
          if(ret<0)
          {
            LOG(FATAL,"pthread_create new thread failed");
            exit(0);
      }
        }
      LOG(INFO,"UdpChat service start syccess");
      while(1)
      {
        struct sockaddr_in cliaddr;
        socklen_t cliaddrlen=sizeof(cliaddr);
        int newsock=accept(TcpSock_,(struct sockaddr*)&cliaddr,&cliaddrlen);
        if(newsock<0)
        {
          LOG(ERROR,"accept failed");
          continue;
        }

        LoginConnect* lc =new LoginConnect(newsock,(void*)this);
        if(!lc)
        {
            LOG(ERROR,"create loginconnect failed");
            continue;
        }
        //:创建线程处理登录
        pthread_t tid;
        int ret=pthread_create(&tid,NULL,LoginReg,(void*)lc);
        if(ret<0)
        {
          LOG(ERROR,"create LoginReg connect failed");
          continue;
        }
        
      }
      LOG(INFO,"UdpChat Service start Success");
      }
  private:
   static void* ProductStart(void* arg)
    {
      pthread_detach(pthread_self());
      ChatSever* cs=(ChatSever*)arg;
      while(1)
      {
          cs->Recv();
      }
      return NULL;
    }
   static void* ConsumeStart(void* arg)
   {
     pthread_detach(pthread_self());
     ChatSever* cs=(ChatSever*)arg;
     while(1)
     {
        cs->BroadcastMsg();

     }
     return NULL;
   }
  private:
   static void* LoginReg(void* arg)
   {
     pthread_detach(pthread_self());
     LoginConnect* lc=(LoginConnect*)arg;
    ChatSever* cs=(ChatSever*)lc->GetServer();
     //:登录注册请求
     //:请求从cli端来
     char QuestType;//:请求类型
    ssize_t recvsize= recv(lc->GetTcpSock(),&QuestType,1,0);
    if(recvsize<0)
    {
      LOG(ERROR,"recvive tag failed");
    }
    else if(recvsize==0)
    {
      LOG(ERROR,"client shutdown connect");
      return NULL;
    }
    uint32_t UserId=-1;
    int UserStatus=-1;
    //:正常接收到一个请求标识
    switch(QuestType)
    {
      case REG:
        //:使用用户管理模块的注册
          UserStatus=cs->DealRegister(lc->GetTcpSock(),&UserId);
        break;
      case LOGIN:
        {
        //:使用用户管理模块的登录
       UserStatus=cs->DealLogin(lc->GetTcpSock());
        break;
        }
      case LOGINOUT:
        //:使用用户管理的退出登录
        cs->DealLoginOut();
        break;
      default:
        LOG(ERROR,"recvive Request types failed");
        break;
    }
  //:响应(send)
  ReplyInfo ri;
  ri.Status=UserStatus;
  ri.UserId_=UserId;
  ssize_t sendsize=send(lc->GetTcpSock(),&ri,sizeof(ri),0);
  if(sendsize<0)
  {
    //:如果数据发送失败了就要考虑应用层重新发送
    LOG(ERROR,"sendMsg failed");
  }
  LOG(INFO,"sendMsg success");
  //:将tcp连接释放掉
  close(lc->GetTcpSock());
  delete lc;
   return NULL;
   }
   void Recv()
   {
     char buf[10240]={0};
     struct sockaddr_in cliaddr;
     socklen_t cliaddrlen=sizeof(struct sockaddr_in);
     int recvsize=recvfrom(UdpSock_,buf,sizeof(buf)-1,0,(struct sockaddr*)&cliaddr,&cliaddrlen);
     if(recvsize<0)
     {
       LOG(ERROR,"recv failed");
     }
     else
     {
        std::string msg;
        msg.assign(buf,recvsize);
        LOG(INFO,msg);
        MsgPool_->PushMsg(msg);
     }
   }
   void BroadcastMsg()
   {
     //:获取要给哪个用户进行发送
     //：获取发送的内容
    //: SendMsg()
     std::string msg;
      MsgPool_->PopMsg(&msg);
   }
   //:给一个客户端发送消息的接口
   void SendMsg(const std::string msg,struct sockaddr_in& cliaddr,socklen_t &len)
   {
     size_t sendsize=sendto(UdpSock_,msg.c_str(),msg.size(),0,(struct sockaddr*)&cliaddr,len);
     if(sendsize<0)
     {
        LOG(ERROR,"send msg failed");
     }
    else
    {
    LOG(INFO,"send msg success");
      
    }
   }

   int DealRegister(int sock,uint32_t* UserId)
   {
      //:接收注册请求
      Reg ri;
      ssize_t recvsize=recv(sock,&ri,sizeof(ri),0);
      if(recvsize<0)
      {
        LOG(ERROR,"Recv RegQuest failed");
        return OFFLINE;
      }
      else if(recvsize==0)
      {
        LOG(ERROR,"client shutdowm connect");
      }
     int ret=UserMAnger->Register(ri.NickName,ri.School,ri.PassWord,UserId);
      //调用用户管理模块进行注册请求的处理
      if(ret==-1)
      {
        return REGF;
      }
      return REGISTERED;
      //返回注册成功后给用户的id
      //返回当前状态
   }
   int DealLogin(int sock)
   {
      struct Login li;
      ssize_t recvsize=recv(sock,&li,sizeof(li),0);
      if(recvsize<0)
      {
        LOG(ERROR,"Recv TagType failed");
        return OFFLINE;
      }
      else if(recvsize==0)
      {
        LOG(ERROR,"client shutdowm connect");
      }
      int ret=UserMAnger->Login(li.UserId,li.PassWord);
      if(ret<0)
      {
        return LOGF;
      }
      return LOGIN;
   }
   int DealLoginOut()
   {

   }
private:
    int UdpSock_;
    int UdpPort_;
    int TcpSock_;
    int TcpPort_;
    //:数据池定义
    MsgPool* MsgPool_;
    //:用户管理模块
    UserManger* UserMAnger;
};
