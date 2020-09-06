#include "search.h"
#include<iostream>
#include<fstream>
#include"../common/Util.hpp"

using namespace std;
namespace search
{
//:index模块相关内容

//把docid就作为数组下标
    const DocInfo* Index::getDocInfo(int64_t docId)
    {
      if(docId>=(int64_t)forwardIndex.size())
      {
        return nullptr;
      }
      return &forwardIndex[docId];
    }
    const InvertedList* Index::getInverted(const string& key)
    {

      auto it=inverteIndex.find(key);
      if(it==inverteIndex.end())
      {
        return nullptr;
      }
      return &it->second;
    }
    //输入内容为之前准备好的raw_input 文件
    //raw_input是一个行文本文件，每行记录对应到一个文档
    //每行包含三个部分使用'\3'分割；分别是title   url   content
    bool Index::build(const string& inputPath)
    {
      cout<<"build start"<<endl;
    //:1.按行读取文件内容
    ifstream file(inputPath.c_str());
    if(!file.is_open())
    {
      cout<<"build open inputPath failed; inputPath="<<endl;
      return false;
    }
    string line;
    while(getline(file,line))
    {
    //2.根据当前行，构造DocInfo对象并插入到正排索引中
    DocInfo* docInfo=BuildForward(line);
    if(docInfo==nullptr)
    {
      continue;
    }
    //3.再进行一步解析DocInfo中的内容，再吧结果构建到倒排索引的结构中
    buildInverted(*docInfo);
    }
    file.close();
    cout<<"build finish"<<endl;
    return true;
    }
   DocInfo* Index::BuildForward(const std::string& line)
    {
    //:1.按照\3针对这一行数据进行切分
    //切分出的结果第一部分为title,第二部分为url，第三部分为content
    vector<string> tokens;
    common::Util::split(line,"\3",&tokens);
    //：2创建DocInfo对象，把这些结构都拼装进去
    DocInfo docInfo;
    docInfo.title=tokens[0];
    docInfo.url=tokens[1];
    docInfo.content=tokens[2];
    //3.把得到的DocInfo对象的指针返回回去，供倒排索引的构造过程来使用
    return &forwardIndex.back();
    }
}
    // end searchcher
