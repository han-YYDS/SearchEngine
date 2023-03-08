/*===============================================================
 *   Copyright (C) 2022 Sangfor Ltd. All rights reserved.
 *   
 *   文件名称：ProtocolParser.h
 *   创 建 者：Haizi
 *   创建日期：2022年08月03日
 *   描    述：
 *
 ===============================================================*/


#ifndef _PROTOCOLPARSER_H_
#define _PROTOCOLPARSER_H_

#include <string>

using std::string;

class ProtocolParser
{
public:
    ProtocolParser() {  }
    
    void doParse(const string &);
    
    string getQueryWords();
    string getResults();

private:
    string _msg;
    string _queryWords;
    string _results;

};


#endif
