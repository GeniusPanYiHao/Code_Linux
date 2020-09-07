#include <iostream>
#include "search.h"

int main()
{
  using namespace std;
  search::Index index; 
  bool ret = index.build("../data/tmp/raw_input");
  if (!ret) 
  {
    cout << "build failed" << endl;
    return 1;

  }
  // 调用 index 中的查正排和查倒排的方法了. 
   auto* invertedList = index.getInverted("filesystem");
   for (const auto& weight : *invertedList) {
   cout << weight.docId << "; " << weight.weight << ";" << weight.word << endl;
   // 根据 docId 再查查正排, 看看查到的结果对不对. 
    const search::DocInfo* docInfo = index.getDocInfo(weight.docId);
   cout << "docId: " << docInfo->docId << "; title: " << docInfo->title<< " url: " << docInfo->url << endl;
   cout << docInfo->content << endl;
  cout << "=========================================" << endl;
   }
  return 0;
}                                      
