/*===============================================================
 *   Copyright (C) 2022 Sangfor Ltd. All rights reserved.
 *   
 *   文件名称：MyRedis.h
 *   创 建 者：Haizi
 *   创建日期：2022年08月04日
 *   描    述：
 *
 ===============================================================*/

#ifndef __TESTREDIS_H__
#define __TESTREDIS_H__

#include <string.h>
#include <stdio.h>
#include <hiredis/hiredis.h>
#include <iostream>
#include <string>

using std::cout;
using std::endl;
using std::string;

class MyRedis
{
public:
    MyRedis()
    {
    }

    ~MyRedis()
    {
        _pConnect = nullptr;
        _pReply = nullptr;
    }

    bool connect(string host, int port)
    {
        _pConnect = redisConnect(host.c_str(), port);
        if(_pConnect != nullptr && _pConnect->err)
        {
            cout << "connect error : " << _pConnect->errstr << endl;
            return false;
        }
        return true;
    }

    string get(string key)
    {
        _pReply = (redisReply*)redisCommand(_pConnect, "GET %s", key.c_str());
        string str = _pReply->str;
        cout << "from redis: " << str << endl;

        freeReplyObject(_pReply);
        _pReply = nullptr;

        return str;
    }

    void set(string key, string value)
    {
        _pReply = (redisReply*)redisCommand(_pConnect, "SET %s %s",
                                            key.c_str(), value.c_str());
        freeReplyObject(_pReply);
        _pReply = nullptr;
    }

    int exists(string key)
    {
        _pReply = (redisReply*)redisCommand(_pConnect, "exists %s", key.c_str());
        int result = _pReply->integer;
        freeReplyObject(_pReply);
        _pReply = nullptr;
        return result;
    }
    
    bool select(string key)
    {
        _pReply = (redisReply*)redisCommand(_pConnect, "select %s", key.c_str());
        int result = _pReply->integer;
        freeReplyObject(_pReply);
        _pReply = nullptr;
        return result;
    }

private:
    redisContext* _pConnect;
    redisReply* _pReply;
};

#endif

