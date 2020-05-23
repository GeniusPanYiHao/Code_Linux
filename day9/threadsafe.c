#include<stdio.h>
#include<pthread.h>
#include<unistd.h>
#include<string.h>
#include<stdlib.h>
#define THREADCOUNT 4
int tickets=100;
pthread_mutex_t g_mutex;
void* thread_start(void* arg)
{
  (void)arg;
  while(1)
  {
    //:加锁
    pthread_mutex_lock(&g_mutex);
    if(tickets>0)
    {
      tickets--;
      printf("i am thread [%p],i have tickets num is [%d]\n",pthread_self(),tickets);
    }
    else
    {
      pthread_mutex_unlock(&g_mutex);
      break;  
  }
    pthread_mutex_unlock(&g_mutex);
  }
  return NULL;
}
int main()
{
  pthread_t tid[THREADCOUNT];
  int i=0;
  pthread_mutex_init(&g_mutex,NULL);
  for(i;i<THREADCOUNT;i++)
  {
    pthread_create(&tid[i],NULL,thread_start,NULL);
  }
  for(i;i<THREADCOUNT;i++)
  {
    pthread_join(tid[i],NULL);
  }
  pthread_mutex_destroy(&g_mutex);
  return 0;
}


