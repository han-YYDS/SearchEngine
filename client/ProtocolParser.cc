/*===============================================================
 *   Copyright (C) 2022 Sangfor Ltd. All rights reserved.
 *   
 *   文件名称：ProtocolParser.cc
 *   创 建 者：Haizi
 *   创建日期：2022年08月03日
 *   描    述：
 *
 ===============================================================*/

#include "ProtocolParser.h"
#include <iostream>
#include <nlohmann/json.hpp>

using std::cout;
using std::endl;

using json = nlohmann::json;

void ProtocolParser::doParse(const string & msg)
{
    _msg = msg;
    
    //cout << "正在解析：" << endl << _msg << endl;
    
    nlohmann::json j = json::parse(_msg);

    _queryWords = j.at("queryWords").get<string>();
    _results = j.at("results").get<string>();

}


string ProtocolParser::getQueryWords()
{
    return _queryWords;
}

string ProtocolParser::getResults()
{
    return _results;

}
