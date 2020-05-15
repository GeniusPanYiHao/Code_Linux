#include<stdio.h>
#include<unistd.h>
#include<string.h>
int main()
{
  int fd[2];
  int ret=pipe(fd);
  if(ret!=0)
  {
    printf("crea tpipe failed\n");
    return 0;
  }
  //:需要先创建好管道在创建进程
  ret=fork();
  if(ret<0)
  {
    perror("fokr");
    return 0;
  }
  else if(ret==0)
  {
    //：子进程
    write(fd[1],"test",4);
    while(1)
    {
      sleep(1);
    }
  }
  else
  {
    //:父进程
    sleep(1);
    char arr [1024];
    read(fd[0],arr,sizeof(arr)-1);
    printf("I am father fd[0] read [%s]\n",arr);
    memset(arr,'0',1024);
    //：是否把数据取走了
    //:read在这边等待，阻塞
    read(fd[0],arr,sizeof(arr)-1);
    printf("I am father fd[0] read [%s]\n",arr);
    while(1)
    {
      sleep(1);
    }
  }
   return 0;
}
