#ifndef __KeyRecommander_H__
#define __KeyRecommander_H__

#include"TcpConnection.h"

#include<string>
#include<vector>
#include<queue>
#include<set>

using std::string;
using std::vector;
using std::priority_queue;
using std::set;

class Myresult
{
public:
    Myresult(const string & word, int freq, int dist)
    :_word(word)
    ,_freq(freq)
    ,_dist(dist)
    {

    }
public:
    string _word;
    int _freq;
    int _dist;
};

class MyCompare
{
public:
    bool operator()(const Myresult& lhs, const Myresult& rhs)
    {
        if(lhs._dist > rhs._dist)
        {
            return true;
        }
        if(lhs._dist == rhs._dist)
        {
            if(lhs._freq < rhs._freq)
            {
                return true;
            }
            if(lhs._freq == rhs._freq)
            {
                if(lhs._word > rhs._word)
                {
                    return true;
                }
                return false;
            }
            return false;
        }
        return false;
    }
};

class KeyRecommander
{
    /* using TcpConnectionPtr = shared_ptr<TcpConnection>; */
public:
    KeyRecommander() = default;
    ~KeyRecommander() = default;
    string execute(const string &);
    /* void setQueryWords(string ); */
    /* void setConn(const TcpConnectionPtr & conn); */
private:
    void queryIndexTable();
    void statistic(set<int>& iset);
    int distance(const string & rhs);
    string response();
    void handleResult();//将priority_queue中的元素去重后存在vector<string>中

    size_t nBytesCode(const char ch);//一个字符占的字节数
    size_t length(const string& str);//字符串的长度
    int triple_min(const int &a, const int &b, const int &c);
        
private:
    vector<string> _finallyResult;
    string _queryWord;
    /* TcpConnectionPtr _conn; */
    priority_queue<Myresult, vector<Myresult>, MyCompare> _resultQue;
};




#endif
