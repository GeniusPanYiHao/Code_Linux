#include<stdio.h>
#include<unistd.h>
int main()
{
  int count=0;
  while(1)
  {
    printf("i am a single process count:[%d]\n",count);
    count++;
    usleep(10000);
  }
}
