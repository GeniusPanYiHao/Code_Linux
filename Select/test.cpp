#include<iostream>
#include<sys/select.h>
#include<unistd.h>
int main()
{
begin:
  fd_set readfd;
  FD_ZERO(&readfd);
  FD_SET(0,&readfd);
  struct timeval time;
  time.tv_sec=3;
  time.tv_usec=2000;
  int ret=select(1,&readfd,NULL,NULL,&time);
  if(ret<0)
  {
    printf("chaoshi\n");
  }
  else if(ret==0)
  {
    printf("jianshi failed\n");
    ret=FD_ISSET(0,&readfd);
    if(ret==0)
    {
      printf("0 fd is not in readfd\n");
      goto begin;
    }
    else if(ret!=0)
    {
      printf("0 is not in readfd\n");
      return 0;
    }
  }
  printf("ret: %d",ret);
  if(FD_ISSET(0,&readfd)!=0)
  {
    char buf[1024]={0};
    read(0,buf,sizeof(buf)-1);
    printf("buf：%s\n",buf);
  }
  FD_CLR(0,&readfd);
}
