#include<stdio.h>
#include<pthread.h>
#include<unistd.h>

void* thread_start(void* arg)
{
  printf("我是一个线程\n");
  pthread_exit(NULL);
  sleep(25);
  printf("我要调用return返回了\n");
    return NULL;
}
int main()
{
  pthread_t pid;
  pthread_create(&pid,NULL,thread_start,NULL);
  while(1)
  {
    printf("我刚刚创建了一个进程现在不知道怎么样了\n");
    sleep(1);
  }
  return 0;
}
