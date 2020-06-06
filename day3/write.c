#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>

int main()
{
  //:在打开之前就可以调用makefifo函数来创建命名管道
  int fd=open("./a.txt",O_RDWR);
  if(fd<0)
  {
    perror("open");
    return 0;
  }
  while(1)
  {
  write(fd,"day3",4);
  sleep(1);
  }
  return 0;
}
