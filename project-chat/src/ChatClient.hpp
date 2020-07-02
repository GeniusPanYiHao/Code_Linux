#pragma once
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<sys/types.h>
#include<iostream>
#include<string>
#include<string.h>
#include"json/json.h"
#include"LogSever.hpp"
#include"ConnectInFo.hpp"
#define UDPPORT 17777
#define TCPPORT 17779

struct Myself
{
  std::string NickName_;
  std::string School_;
  std::string Password_;
  uint32_t UserId_;
};
class ChatClient
{
  public:
    ChatClient(std::string SvrIp="127.0.0.1")
    {
      UdpSock_=-1;
      UdpPort_=UDPPORT;
      TcpSock_=-1;
      TcpPort_=TCPPORT;

      SvrIp_=SvrIp;
    }
    ~ChatClient()
    {
      if(UdpSock_>0)
      {
        close(UdpSock_);
      }
    }
    void Init()
    {
      UdpSock_=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
      if(UdpPort_<0)
      {
        LOG(ERROR,"client create udp socket failed")<<std::endl;
        exit(1);
      }
    }
    bool Connect2Server()
    {
      //:创建tcp socket
      TcpSock_=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
      if(TcpSock_<0)
      {
        LOG(ERROR,"client create tcp socket failed")<<std::endl;
        exit(2);
      }

      //:服务端的地址信息填充
      struct sockaddr_in peeraddr;
      peeraddr.sin_family=AF_INET;
      peeraddr.sin_port=htons(TcpPort_);
      peeraddr.sin_addr.s_addr=inet_addr(SvrIp_.c_str());

      int ret=connect(TcpSock_,(struct sockaddr*)&peeraddr,sizeof(peeraddr));
      if(ret<0)
      {
        perror("connect");
        LOG(ERROR,"Connect Server failed")<<SvrIp_<<":"<<TcpPort_<<std::endl;
        return false;
      }
      return true;
    }
    bool Register()
    {
      if(!Connect2Server())
      {
        return false;
      }
      char type=REG;
      ssize_t send_size=send(TcpSock_,&type,1,0);
      if(send_size<0)
      {
        LOG(ERROR,"Send Register type failed")<<std::endl;
        return false;
      }
      //:发送注册内容
      struct Reg ri;
      std::cout<<"Please Enter Your NickName:";
      std::cin>>ri.NickName;
      std::cout<<"Please Enter Your School";
      std::cin>>ri.School;
      while(1)
      {
        std::cout<<"Please Enter Your password:";
        std::string PasswdOne;
        std::cin>>PasswdOne;
        std::cout<<"Please Enter Your Password again";
        std::string PasswdTwo;
        std::cin>>PasswdTwo;
        if(PasswdOne==PasswdTwo)
        {
          strcpy(ri.PassWord,PasswdOne.c_str());
          break;
        }
        else
        {
          printf("The passwords did not match twice");
        }
      }
      send_size=send(TcpSock_,&ri,sizeof(ri),0);
      if(send_size<0)
      {
        LOG(ERROR,"Send Register type failed")<<std::endl;
        return false;
      }
      //:3.解析应答状态和获取用户ID
      struct ReplyInfo resp;
      ssize_t recv_size=recv(TcpSock_,&resp,sizeof(resp),0);
      if(recv_size<0)
      {
        LOG(ERROR,"Recv Register response failed")<<std::endl;
        return false;
      }
      else if(recv_size == 0)
      {
        LOG(ERROR, "Peer shutdown connect") << std::endl;
        return false;

      }

      if(resp.Status != REGS)
      {
        LOG(ERROR, "Register Failed") << std::endl;
        return false;

      }
      LOG(INFO, "Register success UserId is ") << resp.UserId_ << std::endl;
      //:保存服务端返回的UserID;
      me_.NickName_=ri.NickName;
      me_.School_=ri.School;
      me_.Password_=ri.PassWord;
      me_.UserId_=resp.UserId_;
      close(TcpSock_);
      return true;
    }
    bool Login()
    {
      if(!Connect2Server())
      {
        return false;
      }
      //:发送登录标识
      char type = LOGIN;
      ssize_t send_size = send(TcpSock_, &type, 1, 0);
      if(send_size < 0)
      {
        LOG(ERROR, "Send Login type failed") << std::endl;
        return false;

      }
      //;发送登录数据
      struct Login li;
      li.UserId= me_.UserId_;
      strcpy(li.PassWord, me_.Password_.c_str());                              
      send_size = send(TcpSock_, &li, sizeof(li), 0);
      if(send_size < 0)                                               
      {
        LOG(ERROR, "Send Login data failed") << std::endl;
        return false;                                                                      
      }
      //:解析登录状态
      struct ReplyInfo resp;
      ssize_t recv_size = recv(TcpSock_, &resp, sizeof(resp), 0);
      if(recv_size < 0)
      {
        LOG(ERROR, "Recv Login response failed") << std::endl;
        return false;

      }
      else if(recv_size == 0)
      {
        LOG(ERROR, "Peer shutdown connect") << std::endl;
        return false;

      }

      if(resp.Status != LOGIN)
      {
        LOG(ERROR, "Login failed Status is ") << resp.Status << std::endl; 
        return false;

      }
      LOG(INFO, "Login success") << std::endl;
      return true;
    }
    //:udp数据的收发
    bool SendMsg(const std::string& msg)
    {
      struct sockaddr_in peeraddr;
      peeraddr.sin_family = AF_INET;
      peeraddr.sin_port = htons(UdpPort_);
      peeraddr.sin_addr.s_addr = inet_addr(SvrIp_.c_str());
      ssize_t send_size = sendto(UdpSock_, msg.c_str(), msg.size(), 0, (struct sockaddr*)&peeraddr, sizeof(peeraddr));
      if(send_size < 0)
      {
        LOG(ERROR, "Send Msg to Server Failed\n");
        return false;                                                             
      }
      return true;

    }
    bool RecvMsg(std::string* msg)
    {
      char buf[MESSAGE_MAX_SIZE];
      memset(buf, '\0', sizeof(buf));
      struct sockaddr_in svraddr;
      socklen_t svraddrlen = sizeof(svraddr);
      ssize_t recv_size = recvfrom(UdpSock_, buf, sizeof(buf) - 1, 0, (struct sockaddr*)&svraddr, &svraddrlen);
      if(recv_size < 0)
      {
        LOG(ERROR, "recv msg from server failed");
        return false;

      }
      (*msg).assign(buf, recv_size);
      return true;

    }

    Myself& GetMySelf()
    {
      return me_;

    }

    void PushUser(std::string& user_info)
    {
      auto iter = OnlineUser_.begin();
      for(; iter != OnlineUser_.end(); iter++)
      {
        if(*iter == user_info)
        {
          return;

        }

      }

      OnlineUser_.push_back(user_info);
      return;

    }

    std::vector<std::string>& GetOnlineUser()
    {
      return OnlineUser_;

    }
  private:
    //:发送正常的业务数据
    int  UdpSock_;
    int UdpPort_;
    //:处理登录注册请求的
    int TcpSock_;
    int TcpPort_;
    //:保存服务端的ip
    std::string SvrIp_;
    //:客户端的信息
    Myself me_;

    //:保存在线用户
    std::vector<std::string> OnlineUser_;


};

