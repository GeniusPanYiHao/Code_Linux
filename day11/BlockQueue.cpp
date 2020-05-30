#include<stdio.h>
#include<pthread.h>
#include<iostream>
#include<queue>

#define THREADCOUNT 4
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
      pthread_mutex_lock(&QueueMutex);
      while(Queue.empty())
      {
        pthread_cond_wait(&ConsumeCond,&QueueMutex);
      }
     *date=Queue.front();
      Queue.pop();
      pthread_mutex_unlock(&QueueMutex); 
      pthread_cond_signal(&ProduceCond);
      pthread_cond_signal(&ConsumeCond);
    }
    int push(int &date)
    {
      pthread_mutex_lock(&QueueMutex);
      while(isFull())
      {
        pthread_cond_wait(&ProduceCond,&QueueMutex);
      }
      Queue.push(date);
      pthread_mutex_unlock(&QueueMutex);
      pthread_cond_signal(&ConsumeCond);
      pthread_cond_signal(&ProduceCond);
    }

  private:
    bool isFull()
    {
      if(Queue.size()==Capacity)
      {
        return true;
      }
      else
        return false;
    }
  private:
    std::queue<int> Queue;
    size_t Capacity;

    pthread_mutex_t QueueMutex;

    pthread_cond_t ConsumeCond;
    pthread_cond_t ProduceCond;
};
    void* Consume_Start(void* arg)
    {
      BlockQueue* bq=(BlockQueue*)arg;
      while(1)
      {
        int data;
        bq->pop(&data);
        printf("Consume_Start[%p][%d]\n",pthread_self(),data);
      }

      return NULL;
    }
    void* Produce_Start(void* arg)
    {
      BlockQueue* bq=(BlockQueue*)arg;
      int i=0;
      while(1)
      {
          bq->push(i);
          printf("Produce_Start[%p][%d]\n",pthread_self(),i);

          i++;
      }
      return NULL;
    }


int main()
{
    pthread_t con_tid[THREADCOUNT],pro_tid[THREADCOUNT];
      BlockQueue* bq=new BlockQueue(5);

    int i=0;
    for(;i<THREADCOUNT;i++)
    {
      int ret=pthread_create(&con_tid[i],NULL,Consume_Start,(void*)bq);
      if(ret!=0)
      {
        printf("create thread failed\n");
        return 0;
      }
      ret=pthread_create(&pro_tid[i],NULL,Produce_Start,(void*)bq); 
      if(ret!=0)
      {
        printf("create thread failed\n");
      return 0;
      }
    }
    delete bq;
    bq=NULL;
    return 0;
}
