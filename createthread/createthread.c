#include<stdio.h>
#include<pthread.h>
#include<unistd.h>
void* thread_start(void *arg)
{
  printf("i am new thread!\n");
  sleep(15);
  return  NULL;
}


int main()
{
  pthread_t tid;
  pthread_create(&tid,NULL,thread_start,NULL);
  while(1)
  {
    sleep(1);
    printf("i am main thread\n");
  }
  return 0;
}
