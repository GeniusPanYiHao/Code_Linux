#include<iostream>
#include<fstream>
#include<algorithm>
#include "../common/Util.hpp"

#include"search.h"

using namespace std;
namespace search
{
  //:index模块相关内容
  //:专门写一个Index类的构造函数，来初始化jieba对象
  const char* const DICT_PATH = "../jieba_dict/jieba.dict.utf8";
  const char* const HMM_PATH = "../jieba_dict/hmm_model.utf8";
  const char* const USER_DICT_PATH = "../jieba_dict/user.dict.utf8";
  const char* const IDF_PATH = "../jieba_dict/idf.utf8";
  const char* const STOP_WORD_PATH = "../jieba_dict/stop_words.utf8";
  Index::Index() : jieba(DICT_PATH,
      HMM_PATH,
      USER_DICT_PATH,
      IDF_PATH,
      STOP_WORD_PATH)
  {

  }
//把docid就作为数组下标
    
    const DocInfo* Index::getDocInfo(int64_t docId)
    {
      if(docId >= (int64_t)forwardIndex.size())
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
      cout<<"build open inputPath failed; inputPath="<<inputPath<<endl;
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
    //:每个100个docInfo对象打印一条日志
    if(docInfo->docId%100==0)
    {
      cout<<"Index Build docId:"<<docInfo->docId<<endl;
    }
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
    if(tokens.size()!=3)
    {
      return nullptr;
    }
    //：2创建DocInfo对象，把这些结构都拼装进去
    DocInfo docInfo;
    docInfo.docId=forwardIndex.size();
    docInfo.title=tokens[0];
    docInfo.url=tokens[1];
    docInfo.content=tokens[2];

    forwardIndex.push_back(std::move(docInfo));
    //3.把得到的DocInfo对象的指针返回回去，供倒排索引的构造过程来使用
    return &forwardIndex.back();
    }
   void Index::buildInverted(const DocInfo& docInfo)
   {
     struct WordCut
     {
       int TitleCount;
       int ContentCount;

       //：这个默认函数防止后面构造wordCut的时候没有吧属性都填成0
       WordCut():TitleCount(0),ContentCount(0){}
     };
    unordered_map<string,WordCut> wordCutMap;
     //:1.先针对文档标题进行分词
     vector<string> titleTokens;
     cutWord(docInfo.title,&titleTokens);
     //2.根据分词结果，统计每个词在该标题中的出现次数
     for(string word:titleTokens)
     {
        boost::to_lower(word);
        ++wordCutMap[word].TitleCount;
     }
     //  参考现有的搜索引擎，其并没有区分单词大小写，所以在这个项目里，我们也不区分其大小写
     //3.再针对文档正文进行分词
     vector<string> contentTokens;
     cutWord(docInfo.content,&contentTokens);
     //4.根据分词结果，统计每个词在该正文中的出现次数
     for(string word:contentTokens)
     {
        boost::to_lower(word);
        ++wordCutMap[word].ContentCount;
     }
     //5.遍历统计结果(key是词，value就是标题次数和正文次数)，根据统计结果来更新倒排索引。
     //根据词找到倒排中的对应的倒排拉链，构造weight对象，把这个weight插入到拉链中即可
     //此处auto得到的参数是一个std::pair
     for(const auto& wordPair:wordCutMap)
     {
       Weight weight;
       weight.docId=docInfo.docId;
       //：权重的算法：标题中出现的次数*10+正文中出现的次数
       weight.weight=wordPair.second.TitleCount * 10 + wordPair.second.ContentCount;
       weight.word=wordPair.first;
       //:weight对象准备好了，接下来就可以更新倒排拉链了
       //根据当前的词，在倒排索引中找到对应的倒排拉链
       InvertedList& invertedList=inverteIndex[wordPair.first];
       invertedList.push_back(weight);
     }
   }
   void Index::cutWord(const string& word,vector<string>* tokens)
   {
      jieba.CutForSearch(word,*tokens);
   }
}
    // end searchcher
