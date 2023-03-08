/*===============================================================
 *   Copyright (C) 2022 Sangfor Ltd. All rights reserved.
 *   
 *   文件名称：WebPageSearcher.cc
 *   创 建 者：Haizi
 *   创建日期：2022年07月30日
 *   描    述：
 *
 ===============================================================*/


#include "../include/WebPageSearcher.h"
#include "../include/WebPageQuery.h"
#include "../include/TcpConnection.h"
#include "../include/Mylogger.h"


// 构造函数
/* WebPageSearcher::WebPageSearcher(string keys, const TcpConnectionPtr & conn) */
/* :_sought(keys) */
/* ,_conn(conn) */
/* { */

/* } */

// 执行查询
string WebPageSearcher::doQuery(const string & keys){
    LogInfo(keys);
    _sought = keys;

    WebPageQuery query;
    string res = query.doQuery(_sought);
   
    LogInfo(res);
    
    return res;

}


