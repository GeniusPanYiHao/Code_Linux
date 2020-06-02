#include<stdio.h>
#include<iostream>
#include<unistd.h>
#include<pthread.h>
#include<vector>
#include<semaphore.h>
#define SIZE 4
#define THREADCOUNT 1
class RingQueue
{
  public:
    RingQueue()
      :vec(SIZE)
      {
        capacity=SIZE;
        posWrite=0;
        posRead=0;
        //：生产者的信号量初始化时大小应该和数组大小保持一致
        sem_init(&Prosem,0,SIZE);
        sem_init(&Consem,0,0);

        sem_init(&Locksem,0,1);


      }
    ~RingQueue()
    {
      sem_destroy(&Prosem);
      sem_destroy(&Consem);
      sem_destroy(&Locksem);
    }
    void Push(int& date)
    {
      sem_wait(&Prosem);
      sem_wait(&Locksem);
      vec[posWrite]=date;
      posWrite=(posWrite+1)%capacity;
      sem_post(&Locksem);
      sem_post(&Consem);
    }
    void Pop(int* date)
    {
      //:同步
      sem_wait(&Consem);
      sem_wait(&Locksem);
      *date=vec[posRead];
      posRead=(posRead+1)%capacity;
      sem_post(&Locksem);

      sem_post(&Prosem);
    }
  private:
    std::vector<int> vec;
    size_t capacity;
    int posWrite;
    int posRead;

    sem_t Prosem;
    sem_t Consem;

    sem_t Locksem;
};
void* Constart(void* arg)
{
  RingQueue* rq=(RingQueue*)arg;
  int date;
  while(1)
  {
    rq->Pop(&date);
    printf("Constart consume date[%p][%d]\n",pthread_self(),date);
  }
    return NULL;
}
void* Prostart(void* arg)
{
  RingQueue* rq=(RingQueue*)arg;
  int i=0;
  while(1)
  {
    rq->Push(i);
    printf("Prostart Produce date[%p][%d]\n",pthread_self(),i);
    i++;
  }
      return NULL;
}

int main()
{
  pthread_t Contid[THREADCOUNT],Protid[THREADCOUNT];
  RingQueue* rq=new RingQueue();
  int i=0;

  for(i;i<THREADCOUNT;i++)
  {
   int ret= pthread_create(&Contid[i],NULL,Constart,(void*)rq);

   if(ret!=0)
   {
     perror("pthread_create");
     return 0;
   }
  ret=pthread_create(&Protid[i],NULL,Prostart,(void*)rq);
  if(ret!=0)
  {
    perror("pthread_create");
    return 0;
  }
  }
  for(i=0;i<THREADCOUNT;i++)
  {
      pthread_join(Contid[i],NULL);
      pthread_join(Protid[i],NULL);
  }
  delete rq;
  return 0;
}



