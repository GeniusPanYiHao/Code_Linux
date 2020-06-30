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
    UserInfo(const std::string& NickName,const std::string& School,uint32_t& UserId,const std::string& PassWord)
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
    std::string GetPassWord()
    {
      return PassWord_;
    }
    int& GetUserStatus()
    {
      return UserStatus_;
    }
    void SetCliAddrInFo(const struct sockaddr_in& CliAddr)
    {
      memcpy(&Cliaddr_,&CliAddr,sizeof(CliAddr));
    }
    void SetCliAddrLen(const socklen_t& CliAddrLen)
    {
      Cliaddrlen_=CliAddrLen;
    }
    struct sockaddr_in& GetCliAddrInfo()
    {
      return Cliaddr_;
    }
    socklen_t& GetCliAddrLen()
    {
      return Cliaddrlen_;
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
      PrepareUserId_=0;
    }
    ~UserManger()
    {
      pthread_mutex_destroy(&Lock_);
    }
    int Register(const std::string& NickName, const std::string& School, const std::string& Password, uint32_t* UserId)
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
    int Login(const uint32_t& UserId,const std::string& Password)
    {
        if(Password.size()<0)
        {
          return -1;
        }
        int LoginStatus=-1;
        //:返回登录的状态
        //1.现在map中查找是否存在这样的id
        //不存在/存在/密码正确/密码不正确
        std::unordered_map<uint32_t,UserInfo>::iterator iter;
        pthread_mutex_lock(&Lock_);
        iter=UserMap_.find(UserId);
        if(iter!=UserMap_.end())
        {
          //:查找到了
          if(Password==iter->second.GetPassWord())
          {
            //:密码正确
            iter->second.GetUserStatus()=USERLOGINED;
            LoginStatus=0;
          }
        else
        {
          //:密码错误
          LOG(ERROR,"UserId Password is not correct password is");
          LoginStatus=-1;
        }
      }
        else
        {
          //:没有查找到
          LOG(ERROR,"UserID NOT found");
          LoginStatus=-1;
        }
        pthread_mutex_unlock(&Lock_);
        return LoginStatus;
    }
    int LoginOut()
    {

    }
    bool IsLogin(uint32_t UserId,const struct sockaddr_in& CliAddr,const socklen_t& CliAddrlen)
    {
          if(sizeof(CliAddr)<0||CliAddrlen<0)
          {
            return false;
          }
          //:检测当前用户是否存在
          std::unordered_map<uint32_t,UserInfo>::iterator iter;
          pthread_mutex_lock(&Lock_);
          iter=UserMap_.find(UserId);
          if(iter==UserMap_.end())
          {
            pthread_mutex_unlock(&Lock_);
            LOG(ERROR,"User not exist");
            return false;
          }
          //:判断当前用户状态，来判断是否完成登录和注册
          if(iter->second.GetUserStatus()==ONLINE||iter->second.GetUserStatus()==REGISTERED)
          {
            pthread_mutex_unlock(&Lock_);
            LOG(ERROR,"User Status error");
            return false;
          }
          //：判断当前用户是否是第一次发送消息
          if(iter->second.GetUserStatus()==ONLINE)
          {
            pthread_mutex_unlock(&Lock_);
            return true;
          }
          //:第一次发送消息
          if(iter->second.GetUserStatus()==USERLOGINED)
          {
            //:增加地址信息，地址信息长度，改变状态为ONLINE
            iter->second.SetCliAddrInFo(CliAddr);
            iter->second.SetCliAddrLen(CliAddrlen);
            iter->second.SetUserStatus(ONLINE);

            OnlineUserVec_.push_back(iter->second);
          }
          pthread_mutex_unlock(&Lock_);

          return true;

          
    }
    void GetOnlineUserInfo(std::vector<UserInfo>* vec)
    {
      *vec=OnlineUserVec_;
    }
  private:
    //:保存所有注册用户的信息  TCP
    std::unordered_map<uint32_t,UserInfo> UserMap_;
    pthread_mutex_t Lock_;
    //:保存在线用户的信息  Udp
    std::vector<UserInfo> OnlineUserVec_;
    //:预分配的用户id
    uint32_t PrepareUserId_;
};
