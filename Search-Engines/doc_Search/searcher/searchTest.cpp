#include"search.h"
#include<iostream>

int main()
{
  using namespace std;

  //:调用searcher类的相关操作
  
  search::Search searcher;

  bool ret=searcher.init("../data/tmp/raw_input");
  if(!ret)
  {
    cout<<"search init failed"<<endl;
    return 1;
  }
  while(true)
  {
    cout<<"searcher "<<flush;
    string query;
    cin>>query;
    if(!cin.good())
    {
      cout<<"bye!"<<endl;
      break;
    }
    string output;
    searcher.search(query,&output);
    cout<<"output: "<<endl;
    cout<<output<<endl;
  }
  return 0;
}
