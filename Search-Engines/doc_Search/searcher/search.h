#pragma once

#include<string>
#include<stdint.h>
#include<vector>
#include<unordered_map>
#include"cppjieba/Jieba.hpp"

using std::string;
using std::vector;
using std::unordered_map;

namespace search
{
  //：索引相关代码
  //用于构建正排索引
  //正派本质上就是一个DocInfo数组
  struct DocInfo
  {
    int64_t docId;
    string title;
    string url;
    string content;
  };
  //作为倒排索引的value
  //倒排索引也是一个键值对结构，key就是分词结果，value就是weight对象
  struct Weight
  {
    int64_t docId;
    int weight;
    string word;
  };
  //:倒排拉链
  typedef vector<Weight> InvertedList;
  //：表示索引
  //提供的属性(成员变量)主要有两个：正排索引+倒排索引
  //提供的方法(成员函数)主要有三个：查正排，查倒排，构建索引
  class Index
  {
    private:
      vector<DocInfo> forwardIndex;
      unordered_map<string,InvertedList> inverteIndex;
    public:
      //根据文档id获取正排索引内容
      const DocInfo* getDocInfo(int64_t docId);
      //:根据词，获取倒排拉链
      const InvertedList* getInverted(const string& key);
      //：构建索引
      //inputPath就是指向 raw_input这个文件
      bool build(const string& inputPath);
      Index();
      void cutWord(const string& word,vector<string>* tokens);
    private:
      DocInfo* BuildForward(const std::string& line);
      void buildInverted(const DocInfo& docInfo);
      
      cppjieba::Jieba jieba;
  };

  //:搜索模块代码
  
  class Search
  {
    private:
      Index* index;
    public:
      Search():index(new Index())
    {

    }
      bool init(const std::string& inputPath);

      bool search(const std::string query,std::string* result);
    private:
      static string generateDesc(const string& content,const string& word);
  };
}
