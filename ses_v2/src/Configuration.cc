#include "../include/Configuration.h"

#include <iostream>
#include <fstream>
#include <utility>
#include <sstream>

using std::ifstream;
using std::endl;
using std::cerr;
using std::cout;
using std::ifstream;
using std::ofstream;
using std::istringstream;
using std::map;
using std::pair;
using std::make_pair;

void Configuration::genConfigMap()
{
    ifstream ifs(_filepath);
    if(!ifs)
    {
        cerr << "ifs open " << _filepath << " error!" << endl;
        return;
    }

    string line;
    while(getline(ifs, line))
    {
        istringstream iss(line);
        string filename, filepath;
        iss >> filename;
        iss >> filepath;

        if(!filename.empty()){
            cout << "filename = " << filename << endl;
            cout << "filepath = " << filepath << endl;
            _configMap.insert(make_pair(filename,filepath));

        }
    }

    ifs.close();
}

void Configuration::genStopWordList()
{
    string stopFilename = "stop_words_zh.txt";
    string stopFilepath = _configMap.find(stopFilename)->second;
    ifstream ifs(stopFilepath);

    if(!ifs)
    {
        cerr << "ifs open " << stopFilepath << " error!" << endl;
        return;
    }
    string line;
    while(getline(ifs,line))
    {
        _stopWordList.insert(line);
    }
}

set<string> Configuration::getStopWordList()  // 获取停用词集
{
    return _stopWordList;
}

map<string, string> & Configuration::getConfigMap() // 获取配置文件map
{
    return _configMap;
}
