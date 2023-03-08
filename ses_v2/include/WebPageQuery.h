/*===============================================================
 *   Copyright (C) 2022 Sangfor Ltd. All rights reserved.
 *   
 *   文件名称：WebPageQuery.h
 *   创 建 者：Haizi
 *   创建日期：2022年07月30日
 *   描    述：
 *
 ===============================================================*/


#ifndef _WEBPAGEQUERY_H_
#define _WEBPAGEQUERY_H_

#include "WordSegmentation.h"
#include "WebPage.h"

#include <math.h>
#include <unordered_map>
#include <vector>
#include <set>
#include <string>

using std::unordered_map;
using std::vector;
using std::set;
using std::string;
using std::pair;


class WebPageQuery
{
public:
    // 构造函数
    WebPageQuery();
    // 执行查询，返回结果
    string doQuery(const string &);

private:
    // 加载库文件
    void loadLibrary();
    // 计算查询词的权重值
    vector<double> getQueryWordsWeightVector(vector<string> & queryWords);
    // 执行查询
    bool executeQuery(const vector<string> & queryWords, vector<pair<int, vector<double>>> & resultVec);
    // 创建json
    string createJson(vector<int> & docIdVec, const vector<string> & queryWords);
    // 返回没有找到相关的网页 
    string returnNoAnswer();

    // 比较函数
    static bool comp(pair<int, double> x, pair<int, double> y){
        return x.second > y.second; 
    }

    struct SimplePairHash {
        std::size_t operator()(const std::pair<int, double> & p) const {
            return p.first * p.second;
        }
    };

private:
    WordSegmentation _jieba;    //Jieba 分词库对象
    unordered_map<int, WebPage> _pageLib;       //网页库
    unordered_map<int, pair<int, int>> _offsetLib;      //偏移库
    unordered_map<string, set<pair<int, double>>> _invertIndexTable;   //倒排索引表

};


#endif
