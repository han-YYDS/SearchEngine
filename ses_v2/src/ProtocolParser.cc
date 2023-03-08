/*===============================================================
 *   Copyright (C) 2022 Sangfor Ltd. All rights reserved.
 *   
 *   文件名称：ProtocolParser.cc
 *   创 建 者：Haizi
 *   创建日期：2022年08月03日
 *   描    述：
 *
 ===============================================================*/

#include "../include/ProtocolParser.h"
#include <iostream>
#include <nlohmann/json.hpp>

using std::cout;
using std::endl;

using json = nlohmann::json;

void ProtocolParser::doParse(const string & msg)
{
    _msg = msg;
    
    cout << "正在解析：" << endl << _msg << endl;
    
    nlohmann::json j = json::parse(_msg);

    _msgId = stoi(j.at("msgId").get<string>());
    _msgBody = j.at("msgBody").get<string>();

}

int ProtocolParser::getMsgId()
{
    return _msgId;
}

string ProtocolParser::getMsgBody()
{
    return _msgBody;

}
