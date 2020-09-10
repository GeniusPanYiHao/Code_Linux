#include<iostream>
#include<fstream>
#include<algorithm>
#include<jsoncpp/json/json.h>
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

   //:以下是搜索模块的实现
   

   bool Search::init(const std::string& inputPath)
   {
      return index->build(inputPath);
   }

   bool Search::search(const std::string query,std::string* output)
   {
     //：1分词  对查询词进行分词操作
     vector<string> tokens;
     index->cutWord(query,&tokens);
     //：2触发  根据分词结果，查倒排，找到相关文档id
     vector<Weight> allTokenResult;
     for(string word : tokens)
     {
       boost::to_lower(word);
       const auto* invertedList=index->getInverted(word);
       if(invertedList==nullptr)
       {
         //:该词在所有文档中都不存在
         continue;
       }
       //:如果倒排拉链不是空的，则可以把这些结果合并到一个大的数组里
       //为了下一步进行排序，分词结果可能是多个，对应的倒排拉链也有N条
       //需要把N条拉链合并成一个大的拉链(数组)，然后再针对数组进行排序
       
       allTokenResult.insert(allTokenResult.end(),invertedList->begin(),invertedList->end());
     }
     //：3排序  根据该词在文档中的权重，对结果进行排序
     //     按照权重值进行降序排序，需要调用sort的时候指定降序排序
        std::sort(allTokenResult.begin(),allTokenResult.end(),[](const Weight& w1,const Weight& w2)
              {
                return w1.weight>w2.weight;
              }
            );
     //：4构造结果 把最终的结果进行查正排，构造成JSON格式的数据
     //  为了把结果构造成json格式字符串，需要用到JSON库 jsoncpp
     //  Json::Value类既可以当成vector，也能当成一个map来使用
     //  results是最终的搜索结果，里面有N条记录，results相当于一个数组
        Json::Value results;
        for(const auto&weight : allTokenResult)
        {
          //:weight中只包含docid,咱们需要根据docId在正排索引中查找
          // 查到相关内容，再构成JSON格式的数据
          const auto* docInfo=index->getDocInfo(weight.docId);
          Json::Value result;
          result["title"]=docInfo->title;
          result["url"]=docInfo->url;
          result["desc"]=generateDesc(docInfo->content,weight.word);
          results.append(result);
        }
        //:需要把Json::Value对象转化为字符串，写入到output这个参数中
        //使用write类把Json::Value转化为字符串
        //使用Reader类把字符串转化为Json::Value
        Json::FastWriter writer;
        *output=writer.write(results);
        return true;
   }
  string Search::generateDesc(const string& content,const string& word)
  {
    //:生成描述信息
    //:把正文中包含word的片段摘取出一部分来作为返回结果
    //1.先找到word在content中的位置
    int64_t pos=content.find(word);
    if(pos==(int64_t)string::npos)
    {
      return "";
    }
    //2.以该word出现位置为中心，往前找60个字符，以这个位置为基准，再往后找160个字符
    int64_t begin=pos<60?0:pos-60;
    if(begin+160>=(int64_t)content.size())
    {
      //:如果词出现在末尾，就少截点内容
      return content.substr(begin);
    }
    else
    {
      string desc=content.substr(begin,160);
      desc[desc.size()-1]='.';
      desc[desc.size()-2]='.';
      desc[desc.size()-3]='.';
      return desc;
     }
  }
}
    // end searchcher
