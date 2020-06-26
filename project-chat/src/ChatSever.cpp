#include"ChatSever.hpp"

int main()
{
  ChatSever cs;
  cs.InitSever();
  cs.Start();
  while(1)
  {
    sleep(1);
  }
  return 0;
}
