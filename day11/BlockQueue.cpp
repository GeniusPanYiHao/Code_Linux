#include<stdio.h>
#include<pthread.h>
#include<iostream>
#include<queue>
class BlockQueue 
{
  public:
    BlockQueue(int capacity)
    {
      Capacity=capacity;
      pthread_mutex_init(&QueueMutex,NULL);
      pthread_cond_init(&ConsumeCond,NULL);
      pthread_cond_init(&ProduceCond,NULL);
    }
    ~BlockQueue()
    {

      pthread_mutex_destroy(&QueueMutex);
      pthread_cond_destroy(&ConsumeCond);
      pthread_cond_destroy(&ProduceCond);  
    }
    int pop(int* date)
    {
     *date=Queue.front();
      Queue.pop();
    }
    int push(int &date)
    {
      Queue.push(date);
    }

  private:
    std::queue<int> Queue;
    size_t Capacity;

    pthread_mutex_t QueueMutex;

    pthread_cond_t ConsumeCond;
    pthread_cond_t ProduceCond;


};
