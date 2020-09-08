#include<iostream>
#include<string>
#include<vector>
#include"cpp-httplib/httplib.h"
#include"../searcher/search.h"
int main()
{
  using namespace std;
  using namespace httplib;
  //1.初始化搜索模块
  search::Search searcher;
  bool ret=searcher.init("../data/tmp/raw_input");
  if(!ret)
  {
    cout<<"searcher init failed"<<endl;
    return 1;
  }

  //：1.初始化服务器模块
  //    约定客户端通过query这样的参数来传递查询词
  //    浏览器进行搜索使用url形如：
  //    http://192.168.183.139:10000/searcher?query=filesystem
  Server sever;
  sever.Get("/searcher",[&searcher](const Request& req,Response& resp)
      {
      //：1.获取到query参数
      bool ret=req.has_param("query");
      if(!ret)
      {
        resp.set_content("query parm miss!","text/plan");
        return;
      }
      string query=req.get_param_value("query");

      //：2.使用searcher对象来进行搜索
      string result;
      searcher.search(query,&result);

      resp.set_content(result,"application/json");
      });

      sever.listen("0.0.0.0",10000);

      return 0;
}

