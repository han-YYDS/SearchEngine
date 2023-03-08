/*===============================================================
 *   Copyright (C) 2022 Sangfor Ltd. All rights reserved.
 *   
 *   文件名称：WebPageSearcher.h
 *   创 建 者：Haizi
 *   创建日期：2022年07月30日
 *   描    述：
 *
 ===============================================================*/

#include <string>
#include <memory>

using std::string;
using std::shared_ptr;


/* class TcpConnection; */

/* using TcpConnectionPtr = shared_ptr<TcpConnection>; */

class WebPageSearcher{
public:
    // 构造函数
    WebPageSearcher() = default;
    ~WebPageSearcher() = default;
    /* WebPageSearcher(string keys, const TcpConnectionPtr & conn); */

    /* void setSought(string sought); */
    /* void setConn(const TcpConnectionPtr & conn); */

    // 执行查询
    /* void doQuery(); */
    string doQuery(const string &);

private:
    string _sought;
    /* TcpConnectionPtr _conn; */

};




