#include<stdio.h>
#include<unistd.h>
#include<pthread.h>
#define THREADCOUNT 4

pthread_mutex_t g_mutex;

pthread_cond_t g_eatcond;
pthread_cond_t g_makecond;
int g_noodles=0;
void* EatStart(void* arg)
{
  while(1)
  {
    pthread_mutex_lock(&g_mutex);
    while(g_noodles==0)
    {
      pthread_cond_wait(&g_eatcond,&g_mutex);
    }
    g_noodles--;
    printf("EatStart g_noodles [%d]\n",g_noodles);
    pthread_mutex_unlock(&g_mutex);
    pthread_cond_signal(&g_makecond);
  }
  return NULL;
}
void* MakeStart(void* arg)
{
  while(1)
  {
    pthread_mutex_lock(&g_mutex);                                                                                                                                                    
    while(g_noodles==1)
    {
    pthread_cond_wait(&g_makecond,&g_mutex);
    }
  g_noodles++;
  printf("MakeStart g_noodles [%d]\n",g_noodles);
pthread_mutex_unlock(&g_mutex);
pthread_cond_signal(&g_eatcond);
  }
  return NULL;
}
int main()
{
  pthread_mutex_init(&g_mutex,NULL);
  pthread_cond_init(&g_eatcond,NULL);
  pthread_cond_init(&g_makecond,NULL);
  pthread_t eat_tid[THREADCOUNT],make_tid[THREADCOUNT];
  int i=0;
  for(;i<THREADCOUNT;i++)
  {
    int ret=pthread_create(&eat_tid[i],NULL,EatStart,NULL);
    if(ret!=0)
    {
      printf("create thread failed\n");
      return 0;
    }
    ret=pthread_create(&make_tid[i],NULL,MakeStart,NULL);
    if(ret!=0)
    {
      printf("creat thread failed\n");
      return 0;
    }
  }
  for(i=0;i<THREADCOUNT;i++)
  {
    pthread_join(eat_tid[i],NULL);
    pthread_join(make_tid[i],NULL);
  }
  pthread_mutex_destroy(&g_mutex);
  pthread_cond_destroy(&g_eatcond);
  pthread_cond_destroy(&g_makecond);
  return 0;
}
