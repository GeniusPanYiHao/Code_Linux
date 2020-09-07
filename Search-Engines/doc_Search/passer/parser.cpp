//:此处的paser.cpp会编译一个可执行程序
//：运行指挥就会进行html的预处理过程
#include<iostream>
#include<string>
#include<vector>
#include<fstream>
#include"../common/Util.hpp"
#include<boost/filesystem/path.hpp>
#include<boost/filesystem/operations.hpp>

using namespace std;

//：设置一些全局的配置项，对应到一些目录位置
string g_input_path="../data/input";
//:最终的输出是一个行文本文件
string g_output_path="../data/tmp/raw_input";

//：解析工作，需要准备一个结构体，表示一个文档
struct Doc
{
  //：文档标题，来自html的文件名，最终展示到搜索结果中
  string title;
  //该文档对应的在线版本的文档地址，落地页的地址(需点击的URL)
  string url;
  //进行去标签之后得到的html内容
  string content;
};
bool enumFile(const string &in,vector<string> *out)
{
  //：C++17之前，标准库中，是无法遍历目录的
  //：只能借助操作系统的api或者是使用第三方库(boost)
  //:给boost::filesystem起了个简短的别名
  namespace fs=boost::filesystem;
  fs::path rootPath(in);
  //：判断该目录是否存在
  if(!fs::exists(rootPath))
  {
    cout<<"input path not exists!"<<endl;
    return false;
  }

  //：遍历目录
  //：输入目录中，还包含了一些其他的子目录，子目录中可能还有子目录
  //：递归。boost中有现成的方式，不必写递归代码，就能完成递归工作
  fs::recursive_directory_iterator endIter;//:默认构造函数得到迭代器的end位置
  for(fs::recursive_directory_iterator it(rootPath);it!=endIter;++it)
  {
    //:此处再进行一些判定
    //1.是不是普通文件，如果不是，直接过滤
    if(!fs::is_regular_file(*it))
    {
      continue;
    }
    //2.是不是.html的后缀文件，若不是过滤
    if(it->path().extension()!=".html")
    {
      continue;
    }
    //:得到的结果便是html文件的路径
    out->push_back(it->path().string());
  }
  return true;
}
bool parserTitle(string &html,string *title)
{
  size_t begin=html.find("<title>");
  if(begin==string::npos)
  {
    cout<<"parserTitle failed"<<endl;
    return false;
  }
  size_t end=html.find("</title>");
  if(end==string::npos)
  {
    cout<<"parserTitle failed"<<endl;
    return false;
  }
  begin+=string("<title>").size();
  *title=html.substr(begin,end-begin);
  return true;
}
bool parserUrl(const string& path,string* url)
{
  //:离线文档路径形如: ../data/input/html/foreach.html
  //在线文档路径形如：https://www.boost.org/doc/libs/1_53_0/doc/html/foreach.html
  string head="https://www.boost.org/doc/libs/1_53_0/doc/";
  string tail=path.substr(g_input_path.size());
  *url=head+tail;
  return true;
}
bool parserContent(const string &html,string *content)
{
  bool isContent=true;
  for(auto c:html)
  {
  if(isContent)
  {
    //:当前操作在正文
      if(c=='<')
      {
        //:接下来要切换到标签模式
        isContent=false;
      }
      else
      {
        //：需要把html中的\n干掉，否则行文本就不好使了
        if(c=='\n')
        {
          c=' ';
        }
        //:接下来的操作是正文，将正文内容放到结果中
        content->push_back(c);
      }
  }
  else
  {
    //:当前操作在标签
    //:遇到'>'切换到正文状态，否则其标签中的内容全部忽略
    if(c=='>')
    {
      isContent=true;
    }
  }
 }
  return true;
}
bool parseFile(const string& in,Doc* docInfo)
{
  //:整个解析分为三个环节
  //读取出文件的内容
  string html;
  bool ret=common::Util::read(in,&html);
  if(!ret)
  {
    cout<<"read file failed  "<<in<<endl;
    return false;
  }
  //1.解析文档的标题(文件名)
  ret=parserTitle(html,&docInfo->title);
  if(!ret)
  {
    cout<<"parserTitle failed"<<in<<endl;
    return false;
  }
  //：解析文档的url(文档路径)
  ret=parserUrl(in,&docInfo->url);
  if(!ret)
  {
    cout<<"parserUrl failed"<<endl;
    return false;
  }
  //3.解析文档的正文(去html标签)
  ret=parserContent(html,&docInfo->content);
  if(!ret)
  {
    cout<<"parserContent failed"<<in<<endl;
    return false;
  }
  return true;
}
void writeOutPut(const Doc& docInfo,ofstream& outputfile)
{
  outputfile<<docInfo.title<<"\3"<<docInfo.url<<"\3"<<docInfo.content<<endl;//:中间假的"\3"为分隔符解决粘包问题//:解决沾包问题也可指定长度
}
int main()
{
  //1.枚举出input目录中所有的文件路径，以便后续进行打开和操作
  vector<string> FileList;
  if(!enumFile(g_input_path,&FileList))
  {
      cout<<"enumFile failed"<<endl;
      return 1;
  }
  //2.一次解析出每个文档的结构，进行去标签操作
  //这个对象用来表示最终的输出文件
  ofstream outputFile(g_output_path);
  if(!outputFile.is_open())
  {
    cout<<"opent output file failed"<<endl;
    return 1;
  }
  for(const auto& f:FileList)
  {
  //把结果放到这里，可看到文件遍历的结果
  cout<<"parse file: "<<f<<endl;
  Doc docInfo;
  if(!parseFile(f,&docInfo))
  {
    cout<<"parser file failed"<<f<<endl;
    continue;
  }
  //：3.把结果整理成一个最终的行文本文件
  writeOutPut(docInfo,outputFile);
  }
  outputFile.close();
  return 0;
}

