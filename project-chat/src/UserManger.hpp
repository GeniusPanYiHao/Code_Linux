#pragma once
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <cstring>
#include <string>
#include <unordered_map>
#include <vector>

#include "LogSever.hpp"

#define OFFLINE 0
#define REGISTERED 1
#define USERLOGINED 2
#define ONLINE 3


class UserInfo
{
  public:
    //:注册登录阶段用的是TCP
    UserInfo(const std::string& NickName,const std::string& School,uint64_t& UserId,const std::string& PassWord)
    {
      NickName_=NickName;
      School_=School;
      UserId_=UserId;
      PassWord_=PassWord;
      memset(&Cliaddr_,'0',sizeof(struct sockaddr_in));
      Cliaddrlen_=-1;
      UserStatus_=OFFLINE;
    }
    void SetUserStatus(int status)
    {
      UserStatus_=status;
    }
  private:
  std::string NickName_;
  std::string School_;
  //:用户Id
  uint64_t UserId_;
  std::string PassWord_;
  //:保存的是Udp客户端的地址信息
  struct sockaddr_in Cliaddr_;
  socklen_t Cliaddrlen_;
  //:用户状态
  int UserStatus_;
};

class UserManger
{
  public:
    UserManger()
    {
      UserMap_.clear();
      OnlineUserVec_.clear();
      pthread_mutex_init(&Lock_,NULL);
    }
    ~UserManger()
    {
      pthread_mutex_destroy(&Lock_);
    }
    int Register(const std::string& NickName, const std::string& School, const std::string& Password, uint64_t* UserId)
    {
          if(NickName.size() == 0 || School.size() == 0|| Password.size() == 0)
           {
                  return -1;                              
          }
          pthread_mutex_lock(&Lock_);
          UserInfo userinfo(NickName,School,PrepareUserId_,Password);
          //：更改当前用户的状态改为已注册
          userinfo.SetUserStatus(REGISTERED);
          //:插入到map中
          UserMap_.insert(std::make_pair(PrepareUserId_,userinfo));
          *UserId=PrepareUserId_;
          PrepareUserId_++;
          pthread_mutex_unlock(&Lock_);
          return 0;
    }
    int Register()
    {

    }
    int Login()
    {

    }
    int LoginOut()
    {

    }
  private:
    //:保存所有注册用户的信息  TCP
    std::unordered_map<uint64_t,UserInfo> UserMap_;
    pthread_mutex_t Lock_;
    //:保存在线用户的信息  Udp
    std::vector<UserInfo> OnlineUserVec_;
    //:预分配的用户id
    uint64_t PrepareUserId_;
};
