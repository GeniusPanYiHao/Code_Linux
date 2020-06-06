#include<stdio.h>
#include<sys/stat.h>


int main()
{
  int ret=mkfifo("./b.fifo",0664);
  if(ret<0)
  {
    perror("makefifo");
    return 0;
  }
  return 0;
}
