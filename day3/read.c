#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include<string.h>

int main()
{
  int fd=open("./a.txt",O_RDWR);
  if(fd<0)
  {
    perror("open");
    return 0;
  }

  char buf[1024]={0};
  //:要注意在数组中预留\0的位置
  while(1)
  {
  memset(buf,'\0',1024);
  read(fd,buf,sizeof(buf)-1);
  printf("I am read [%s]\n",buf);
  }
  while(1)
  {
    sleep(1);
  }
  return 0;
}
