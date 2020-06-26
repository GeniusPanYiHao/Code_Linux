#pragma once
#include<iostream>
#include<string>
#include<pthread.h>
#include<queue>

#define MSG_POOL_SIZE 1024

class MsgPool
{
  public:
    MsgPool()
    {
      Capacity=MSG_POOL_SIZE;
      pthread_mutex_init(&MsgQueLock,NULL);
      pthread_cond_init(&ConQue,NULL);
      pthread_cond_init(&ProQue,NULL);
    }
    ~MsgPool()
    {
      pthread_mutex_destroy(&MsgQueLock);
      pthread_cond_destroy(&ConQue);
      pthread_cond_destroy(&ProQue);
    }
    void PushMsg(std::string &msg)
    {
        pthread_mutex_lock(&MsgQueLock);
        while(isFull())
        {
          pthread_cond_wait(&ProQue,&MsgQueLock);
        }
        MsgQue_.push(msg);
        pthread_mutex_unlock(&MsgQueLock);
        pthread_cond_signal(&ConQue);

    }
    void PopMsg(std::string *msg)
    {
      pthread_mutex_lock(&MsgQueLock);
      while(MsgQue_.empty())
      {
        pthread_cond_wait(&ConQue,&MsgQueLock);
      }
     *msg=MsgQue_.front();
      MsgQue_.pop();
      pthread_mutex_unlock(&MsgQueLock);
      pthread_cond_signal(&ProQue);
    }
  private:
    bool isFull()
    {
      if(MsgQue_.size()==Capacity)
      {
        return true;
      }
      return false;
    }
    private:
      std::queue<std::string> MsgQue_;
    //:约束下队列大小，防止异常情况
    size_t Capacity;
    //:互斥
    pthread_mutex_t MsgQueLock;
    //:同步
    //：消费者条件变量
    pthread_cond_t ConQue;
    //:生产者条件变量
    pthread_cond_t ProQue;
};
