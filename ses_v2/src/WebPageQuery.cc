/*===============================================================
 *   Copyright (C) 2022 Sangfor Ltd. All rights reserved.
 *   
 *   文件名称：WebPageQuery.cc
 *   创 建 者：Haizi
 *   创建日期：2022年07月30日
 *   描    述：
 *
 ===============================================================*/


#include "../include/WebPageQuery.h"
#include "../include/Configuration.h"   
#include "../include/Mylogger.h"   

#include <nlohmann/json.hpp>

#include <func.h>
#include <fstream>
#include <istream>
#include <numeric>
#include <algorithm>

using std::cerr;
using std::make_pair;
using std::ifstream;
using std::istringstream;
using std::unordered_set;
using std::to_string;

using json = nlohmann::json;

// 构造函数
WebPageQuery::WebPageQuery(){
    // 加载库文件
    loadLibrary();
}

// 执行查询，返回结果
string WebPageQuery::doQuery(const string & sentence){
    
    LogInfo(sentence);

    // 用 jieba 对查询请求进行分词，生成 vector<string> queryWords
    vector<string> queryWords = _jieba(sentence);

    // 删除停用词
    Configuration *conf = Configuration::getInstance();
    set<string> stopWordList = conf->getStopWordList();

    for(auto it = stopWordList.begin(); it != stopWordList.end(); it++)
    {
        if(find(queryWords.begin(), queryWords.end(), *it) != queryWords.end()){
            cout << *it << endl;
            queryWords.erase(remove(queryWords.begin(), queryWords.end(), *it), queryWords.end());
        }
    }

    cout << "分词结果如下:" << endl;
    for(auto it = queryWords.begin(); it != queryWords.end(); ++it)
    {
        cout << *it << " ";
    }
    cout << endl;
    
    // 对查询词进行计算，生成基准向量 Base = (x1, x2, ...,xn)
    vector<double> base;
    base = getQueryWordsWeightVector(queryWords);

    cout << "base: ";
    for(auto it = base.begin(); it != base.end(); ++it)
    {
        cout << *it << " ";
    }
    cout << endl;

    // 去倒排索引库中查询具有全部查询词的网页
    string msg;
    
    // int 代表 docId, vector<double> 代表向量 Y = (y1,y2, ... yn)
    vector<pair<int, vector<double>>> resultVec;
    
    // 执行查询
    bool ret = executeQuery(queryWords, resultVec);
    if(ret){    //有相关的网页
        // 计算余弦相似度，然后排序，计算公式：
        // X * Y = (x1 * y1 + x2 * y2 +...+ xn * yn)
        // cosθ = (X * Y) / (|X| * |Y|)
        vector<pair<int, double>> cosVec;       //存储文档id和对应的余弦值
        for(auto it = resultVec.begin(); it != resultVec.end(); ++it)
        {
            vector<double> y = it->second;  // 向量 Y
            double xyDP, xNorm, yNorm, cosine;
            
            // 计算点积 X * Y
            xyDP = inner_product(begin(base), end(base), begin(y), 0.0);
            // 计算向量模 |X|
            xNorm = sqrt(inner_product(begin(base), end(base), begin(base), 0.0));
            // 计算向量模 |Y|
            yNorm = sqrt(inner_product(begin(y), end(y), begin(y), 0.0));
            // 计算余弦 cosθ
            cosine = xyDP / (xNorm + yNorm);

            // 文档id和余弦相似度cosine插入到cosVec
            cosVec.push_back(make_pair(it->first, cosine));
        }

        // 对cosVec从大到小排序
        sort(cosVec.begin(), cosVec.end(), comp);

        vector<int> docIdVec;
        for(auto &elem : cosVec)
        {
            docIdVec.push_back(elem.first);
        }

        msg = createJson(docIdVec, queryWords);

    }else{  //没有找到相关的网页
        // 拼接查询词
        string keys;
        for(auto it = queryWords.begin(); it != queryWords.end(); ++it)
        {
            keys.append(*it);
            keys.append(" ");
        }
        
        string results = returnNoAnswer();
        json jMsg = { {"queryWords", keys },
                      {"results", results } };
        
        msg = jMsg.dump();
    }
    
    LogInfo(msg);
    return msg;
}

// 加载库文件
void WebPageQuery::loadLibrary(){
    //
    // 加载偏移库offsetlib
    Configuration *conf = Configuration::getInstance();
    map<string, string> configMap= conf->getConfigMap();
    
    string path = configMap["offset.dat"];
    ifstream offset(path);
    if(!offset)
    {
        cerr << "open offset.dat error!" << endl;
        return;
    }

    // 读入偏移库里的一行
    string line;
    while(getline(offset, line)){
        istringstream iss(line);
        
        // 读入 文章id + 文章的起始地址  + 文章所占字符数
        string docId, start, occupy;
        iss >> docId;
        iss >> start;
        iss >> occupy;

        // 插入 unordered_map<int, pair<int, int>> _offsetLib
        _offsetLib.insert(make_pair(stoi(docId), make_pair(stoi(start), stoi(occupy))));
    
    }
    
    // 关闭文件
    offset.close();
    
    //
    // 加载网页库
    path = configMap["ripepage.dat"];
    ifstream pagelib(path);
    if(!pagelib)
    {
        cerr << "open ripepage.dat error!" << endl;
        return;
    }
    
    // 根据偏移库读入网页
    for(auto it = _offsetLib.begin(); it != _offsetLib.end(); ++it)
    {
        int docId = it->first;      //文档id
        int start = it->second.first;       //文档的起始地址，单位字节数
        int occupy = it->second.second;     //文档的长度，单位字节数
        
        // 从网页库读出一个文档
        pagelib.seekg(start);       // 设置偏移
        char * temp = new char[occupy];
        pagelib.read(temp, occupy);       // 读入occupy个字节
        temp[occupy] = '\0';
        string doc = temp;

        // 创建WebPage对象，插入 unordered_map<int, WebPage> _pageLib
        WebPage page(doc, *Configuration::getInstance(), _jieba);
        _pageLib.insert(make_pair(docId, page));

    }
    
    // 关闭文件
    pagelib.close();
    
    //
    // 加载倒排索引库
    path = configMap["invertIndex.dat"];
    ifstream invertIdx(path);
    if(!invertIdx)
    {
        cerr << "open invertIndex.dat error!" << endl;
        return;
    }
    
    // 读入倒排索引库里的一行
    line = "";
    while(getline(invertIdx, line)){
        istringstream iss(line);
        set<pair<int, double>> weightSet;

        // 读入key为出现在文档中的某个词语，对应的value为包含该词语的文档 ID 的集合以及该词语的权重值 w
        string word, docId, weight;
        iss >> word;
        
        while(iss){
            iss >> docId;
            iss >> weight;
            weightSet.insert(make_pair(stoi(docId), stod(weight)));
        }

        _invertIndexTable.insert(make_pair(word, weightSet));
    }
    
    // 关闭文件
    invertIdx.close();
}

// 计算查询词的权重值
vector<double> WebPageQuery::getQueryWordsWeightVector(vector<string> & queryWords){
    // 词频统计
    unordered_map<string, int> dict;
    for(auto it = queryWords.begin(); it != queryWords.end(); ++it)
    {
        dict[*it]++;
    }

    // 计算向量 Base = (x1, x2, ...,xn)
    vector<double> base;
    double TF = 0.0;
    double DF = 1.0;
    double N = 1.0;
    double IDF, weight;
    for(auto it = dict.begin(); it != dict.end(); ++it)
    {
        TF = (*it).second;
        IDF = log2( N / (DF+1.0) );
        weight = TF * IDF;
        
        base.push_back(weight);
    }

    // 归一化处理
    double baseNorm = sqrt(inner_product(begin(base), end(base), begin(base), 0.0));
    for(auto it = base.begin(); it != base.end(); ++it)
    {
        *it = *it / baseNorm;
    }

    return base;

}

// 执行查询
bool WebPageQuery::executeQuery(const vector<string> & queryWords, vector<pair<int, vector<double>>> & resultVec){
    // vector<pair<int, vector<double>>> resultVec
    // int 代表 docId, vector<double> 代表向量 Y = (y1,y2, ... yn)
    
    // 去除查询词中的重复词
    unordered_set<string> words;
    for(auto it = queryWords.begin(); it != queryWords.end(); ++it)
    {
        words.insert(*it);
    }

    // 查询具有关键词的网页
    unordered_set<pair<int, double>, SimplePairHash> candidateSet;   
    unordered_map<int, int> docTimes;   
    for(auto it = words.begin(); it != words.end(); ++it)
    {
        set<pair<int, double>> temp = _invertIndexTable[*it];
        for(auto it1 = temp.begin(); it1 != temp.end(); ++it1)
        {
            candidateSet.insert(*it1);
            docTimes[it1->first]++;
        }
    }
    
    // 留下具有全部关键词的网页
    for(auto it = docTimes.begin(); it != docTimes.end(); ++it)
    {
       if(it->second == (int)words.size()){ //筛选具有全部关键词的网页
           // 计算具有全部关键词的网页的向量Y
           vector<double> yVec;
           for(auto it1 = candidateSet.begin(); it1 != candidateSet.end(); ++it1)
           {
               if(it1->first == it->first)
               {
                    yVec.push_back(it1->second);
               }
           }

           resultVec.push_back(make_pair(it->first, yVec));
       }
    }

    if(resultVec.empty()){
        return false;
    }else{
        return true;
    }
}

// 创建json
string WebPageQuery::createJson(vector<int> & docIdVec, const vector<string> & queryWords){
    // 返回标题和摘要信息
    json msg;
    
    // 拼接查询词
    string words;
    for(auto it = queryWords.begin(); it != queryWords.end(); ++it)
    {
        words.append(*it);
        words.append(" ");
    }

    msg["queryWords"] = words;

    // 拼接results
    string results;
    for(auto it = docIdVec.begin(); it != docIdVec.end(); ++it)
    {
        auto iter = _pageLib.find(*it);
        if(iter != _pageLib.end()){
            string title = iter->second.getTitle();
            string url = iter->second.getUrl();
  
            iter->second.summary(queryWords);
            string summary = iter->second.getSummary();
    
            results.append("title: ");
            results.append(title);
            results.append("\n");
            results.append("url: ");
            results.append(url);
            results.append("\n");
            results.append("summary: ");
            results.append(summary);
            results.append("\n\n");
        }
    }

    results[results.length()-1] = '\0';
    msg["results"] = results;
    
    return msg.dump();

}

// 返回没有找到相关的网页 
string WebPageQuery::returnNoAnswer(){
    string noAnswer = "啊欧，没有相关的网页...";
    
    return noAnswer;
}

