#include<stdio.h>
#include<unistd.h>
#include<pthread.h>
#include<string.h>
#include<stdlib.h>
void* thread_start(void* arg)
{
 (void)arg;
// pthread_detach(pthread_self());
  char* arr=(char*)malloc(10);
  memset(arr,'\0',10);
  strcpy(arr,"LINUX-day8");
  pthread_exit((void*)arr);
 return NULL;
}
int main()
{
  pthread_t tid;
  pthread_create(&tid,NULL,thread_start,NULL);
  //pthread_detach(tid);
  void* retval;
  pthread_join(tid,&retval);
  printf("retval:%s\n",(char*)retval);
  while(1)
  {
    sleep(5);
    printf("i am main thread\n");
  }
  return 0;
}
