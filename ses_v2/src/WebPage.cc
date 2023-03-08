/*===============================================================
 *   Copyright (C) 2022 Sangfor Ltd. All rights reserved.
 *   
 *   文件名称：WebPage.cc
 *   创 建 者：Haizi
 *   创建日期：2022年07月30日
 *   描    述：
 *
 ===============================================================*/


#include "../include/WebPage.h"
#include "../include/WordSegmentation.h"
#include "../include/Configuration.h"
#include "../include/Mylogger.h"

#include <iostream>
#include <algorithm>

using std::cout;
using std::endl;
using std::find;
using std::remove;

// 构造函数
WebPage::WebPage(string & doc, Configuration & config, WordSegmentation & jieba)
{
    LogInfo(doc);

    processDoc(doc, config, jieba);
    
    LogInfo("finished");
}

// 获取文档id
int WebPage::getDocId()
{
    return _docId;
}

// 获取文档
string WebPage::getDoc()
{
    return _doc;
}

// 获取文档内容
string WebPage::getContent()
{
    return _docContent;
}

// 获取文档标题
string WebPage::getTitle()
{
    return _docTitle;
}

// 获取文档 Url
string WebPage::getUrl()
{
    return _docUrl;
}

// 获取文档 Summary
string WebPage::getSummary()
{
    return _docSummary;
}

// 生成文档摘要
string WebPage::summary(const vector<string> & queryWords)
{
    // TODO 摘要算法
    _docSummary = getNChinese(_docContent, 50);

    /* cout << _docSummary << endl; */
    
    return _docSummary;
}

//提取中文字符 
string WebPage::getNChinese(const string & str, int num)
{
    int MAXLEN = 3*num;
    string intercept;
	int CharCount = 0;
	for (int i = 0; i < str.size(); i++) {
 		if (CharCount <= MAXLEN) {
       	    if (str[i] & 0x80) {
       		    CharCount += 3;
       		    if (CharCount > MAXLEN ) {//对下一个中文是否超出截取范围做判断，超出即不能继续拼接字符串
       			    break;
       		    }
       	        intercept += str[i];
       	        intercept += str[++i];
       	        intercept += str[++i];
            } else {
       	        CharCount += 1;
       	        intercept += str[i];
            }
        }
    }
    return intercept;
}

// 获取文档的词频统计map 
map<string, int> & WebPage::getWordsMap(){
    return _wordsMap;
}

// 对格式化文档进行处理
void WebPage::processDoc(const string & doc, Configuration & config, WordSegmentation & jieba){
    // 初始化数据成员
    // 整篇文档
    _doc = doc;    
    /* cout << _doc << endl << endl; */

    // 文档id
    _docId = stoi(getXmlElementValue(_doc, "docid"));    
    
    // 文档标题
    _docTitle = getXmlElementValue(_doc, "title");    
    
    // 文档URL
    _docUrl = getXmlElementValue(_doc, "url");    
    
    // 文档内容
    _docContent = getXmlElementValue(_doc, "content");    

    // jieba分词
    vector<string> words = jieba(_docContent);
    
    set<string> stopWords = config.getStopWordList();

    // 统计词频，并生成词频最高的20个词
    calcTopK(words, TOPK_NUMBER, stopWords);

}

// 求取文档的 topk 词集
void WebPage::calcTopK(vector<string> & wordsVec, int k, set<string> & stopWordList){
    // 删除停用词  
    for(auto it = stopWordList.begin();it != stopWordList.end(); it++)
    {
        if(find(wordsVec.begin(), wordsVec.end(), *it) != wordsVec.end()){
            wordsVec.erase(remove(wordsVec.begin(), wordsVec.end(), *it), wordsVec.end());
        }

    }
    
    wordsVec.shrink_to_fit();

    // 统计词频
    for(auto it = wordsVec.begin();it != wordsVec.end(); it++)
    {
        _wordsMap[*it]++;
    }

    // 词频vector
    vector<pair<string,int>> wordsFreq(_wordsMap.begin(), _wordsMap.end());
    // 按词频从小到大排序
    sort(wordsFreq.begin(), wordsFreq.end(), comp); 
    
    // 取出词频最高的前 k 个词
    for(int i = 0; i < k; i++){
        if(!wordsFreq.empty()){
            pair<string,int> temp = wordsFreq.back();
            _topWords.push_back(temp.first);
            wordsFreq.pop_back();
        }
    }
}

// 获取xml格式字符串中的节点内容
string WebPage::getXmlElementValue(string xmlStr, string elemName)
{ 
    string beginName, endName;
    beginName.append("<");
    beginName.append(elemName);
    beginName.append(">");
    endName.append("</");
    endName.append(elemName);
    endName.append(">");
    
    /* cout << beginName << endl; */
    /* cout << endName << endl; */

    string value = "";
    while (xmlStr.find(beginName) != string::npos)
    {
        int begin = xmlStr.find(beginName);
        int end = xmlStr.find(endName);
        if (begin >= end) {
            cout << "error in find " << elemName << " Item" << endl;
            exit(1);
        }

        value = xmlStr.substr(begin+beginName.length(), end-begin-beginName.length());

        xmlStr = xmlStr.substr(end+endName.length(), xmlStr.find('\n'));
    }

    return value;
}

// 判断两篇文档是否相等
bool operator==(const WebPage & lhs, const WebPage & rhs){
    return lhs._docId == rhs._docId;
}

// 对文档按id进行排序
bool operator<(const WebPage & lhs, const WebPage & rhs){
    return lhs._docId < rhs._docId;
}
