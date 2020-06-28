#pragma once
#include<iostream>
#include"UserManger.hpp"
#define REG 0
#define LOGIN 1
#define LOGINOUT 2
#define MESSAGE_MAX_SIZE 1024

//:注册信息
struct Reg
{
    char NickName[10];
    char School[10];
    char PassWord[15];
};
//:登录信息
struct Login
{
      uint32_t UserId;//:用户id
      char PassWord[15];
};

enum UserStatus
{
  REGF=0,//:注册失败
  REGS,//:注册成功
  LOGF,//：登录失败
  LOGS//：登录成功
};

//:应答信息
struct ReplyInfo
{
  int Status;
  uint32_t UserId_;
};

class LoginConnect
{
  public:
    LoginConnect(int Sock,void* Server)
    {
      Sock_=Sock;
      Server_=Server;
    }

    int GetTcpSock()
    {
      return Sock_;
    }
    void* GetServer()
    {
      return Server_;
    }
    private:
      int Sock_;
      //:可以保存ChatSever类的实例化指针
      void* Server_;
};
