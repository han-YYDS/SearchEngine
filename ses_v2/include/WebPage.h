/*===============================================================
 *   Copyright (C) 2022 Sangfor Ltd. All rights reserved.
 *   
 *   文件名称：WebPage.h
 *   创 建 者：Haizi
 *   创建日期：2022年07月30日
 *   描    述：
 *
 ===============================================================*/


#ifndef _WEBPAGE_H_
#define _WEBPAGE_H_

#include <string>
#include <vector>
#include <map>
#include <set>
#include <memory>

using std::string;
using std::vector;
using std::map;
using std::set;
using std::pair;

class Configuration;
class WordSegmentation;

class WebPage
{
public:
    // 构造函数
    WebPage(string & doc, Configuration & config, WordSegmentation & jieba);
    
    // 获取文档
    string getDoc();
    // 获取文档 id
    int getDocId();
    // 获取文档 title
    string getTitle();
    // 获取文档 Url
    string getUrl();
    // 获取文档 content
    string getContent();
    // 获取文档 summary
    string getSummary();

    // 获取文档的词频统计 map 
    map<string, int> & getWordsMap();
    
    // 生成文档摘要
    string summary(const vector<string> & queryWords);

    // 判断两篇文档是否相等
    friend bool operator==(const WebPage & lhs, const WebPage & rhs);
    // 对文档按id进行排序
    friend bool operator<(const WebPage & lhs, const WebPage & rhs);

private:
    //只保留中文字符 
    string getNChinese(const string & str, int num);  

    // 对格式化文档进行处理
    void processDoc(const string & doc, Configuration & config, WordSegmentation &);
    // 求取文档的 topk 词集
    void calcTopK(vector<string> & wordsVec, int k, set<string> & stopWordList);
    
    // 获取xml格式字符串中的节点内容
    static string getXmlElementValue(string xmlStr, string elemName);

    // 自定义比较函数
    static bool comp(const pair<string, int> & lhs, const pair<string,int> & rhs) {
        return lhs.second < rhs.second;
    }

private:
    const static int TOPK_NUMBER = 20;

    string _doc;        //整篇文档，包含 xml 在内
    int _docId;         //文档 id
    string _docTitle;   //文档 标题
    string _docUrl;     //文档 URL
    string _docContent;     //文档内容
    string _docSummary;     //文档摘要，需自动生成，不是固定的
    vector<string> _topWords;       //词频最高的前 20 个词
    map<string, int> _wordsMap;     //保存每篇文档的所有词语和词频，不包括停用词

};


#endif
