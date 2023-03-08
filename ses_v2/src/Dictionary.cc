#include"../include/Dictionary.h"
#include<assert.h>
#include<fstream>
#include<iostream>
#include<sstream>

using std::ifstream;
using std::istringstream;

using std::cout;
using std::endl;

/* Dictionary* Dictionary::_pInstance = nullptr; */
/* Dictionary::Autorelease Dictionary::_ar; */

#if 1
void Dictionary::init(const string & cn_dictpath,const string & en_dictpath, const string& cn_indexpath, const string& en_indexpath)
{
    ifstream ifs1;
    ifs1.open(cn_dictpath);
    if(!ifs1.good())
    {
        cout << "Dictionary: ifs1 open file failure" << endl;
    }

    string line1;
    string word1;
    int freq1;

    _dict.emplace_back(std::make_pair(" ", 0));
    while(getline(ifs1, line1), !ifs1.eof())
    {
        istringstream iss(line1);
        iss >> word1 >> freq1;
        _dict.emplace_back(make_pair(word1,freq1));
    }

    ifstream ifs2;
    ifs2.open(en_dictpath);
    if(!ifs2.good())
    {
        cout << "Dictionary: ifs2 open file failure" << endl;
    }

    string line2;
    string word2;
    int freq2;

    while(getline(ifs2, line2), !ifs2.eof())
    {
        istringstream iss(line2);
        iss >> word2 >> freq2;
        _dict.emplace_back(make_pair(word2,freq2));
    }

    ifstream ifs3;
    ifs3.open(cn_indexpath);

    if(!ifs3.good())
    {
        cout << "Dictionary: ifs3 open file failure" << endl;
    }
    string line3;
    string ch;

    while(getline(ifs3, line3), !ifs3.eof())
    {
        istringstream iss(line3);
        set<int> index;

        iss >> ch ;

        int num;
        while(!iss.eof())
        {
            iss >> num;
            index.insert(num);
        }
        _indexTable[ch] = index;
    }

    ifstream ifs4;
    ifs4.open(en_indexpath);

    if(!ifs4.good())
    {
        cout << "Dictionary: ifs4 open file failure" << endl;
    }
    string line4;
    string ch1;

    while(getline(ifs4, line4), !ifs4.eof())
    {
        istringstream iss(line4);
        set<int> index1;

        iss >> ch1 ;

        int num1;
        while(!iss.eof())
        {
            iss >> num1;
            index1.insert(num1);
        }
        _indexTable[ch1] = index1;
    }
    ifs1.close();
    ifs2.close();
    ifs3.close();
    ifs4.close();
}
#endif

vector<pair<string, int>>& Dictionary::getDict()
{
   return _dict;
}

map<string, set<int>> & Dictionary::getIndexRable()
{
    return _indexTable;
}

//"hello"--->"h e l l o"
/* string Dictionary::doQuery(const string & word) */
/* { */
/*     string strTmp; */
/*     for(size_t idx = 0; idx < word.size(); ++idx) */
/*     { */
/*         strTmp.push_back(word[idx]) ;//处理英文 */
/*         strTmp.push_back(' '); */
/*     } */
/*     /1* cout << "strtmp" << strTmp << endl; *1/ */
/*     return strTmp; */
/* } */

//处理中英文，中文眺3个字节，英文跳1个字节
string Dictionary::doQuery(const string & word)
{
    string strTmp;
    size_t charNums = length(word);
    for(size_t idx = 0, i = 0; i != charNums; ++idx, ++i)
    {
        size_t charlen = nBytesCode(word[idx]);
        string subword = word.substr(idx, charlen);

        idx += (charlen - 1);
        strTmp += subword;
        strTmp += " ";
    }
    return strTmp;
}

size_t Dictionary::nBytesCode(const char ch)
{
	if(ch & (1 << 7))
	{
		int nBytes = 1;
		for(int idx = 0; idx != 6; ++idx)
		{
			if(ch & (1 << (6 - idx)))
			{
				++nBytes;	
			}
			else
				break;
		}
		return nBytes;
	}
	return 1;
}  
  
std::size_t Dictionary::length(const std::string &str)
{
	std::size_t ilen = 0;
	for(std::size_t idx = 0; idx != str.size(); ++idx)
	{
		int nBytes = nBytesCode(str[idx]);
		idx += (nBytes - 1);
		++ilen;
	}
	return ilen;
}
#if 0
//test
int main()
{
    Dictionary* pdict =  Dictionary::createInstance();
    /* pdict->init("../file/en_dict.dat", "../file/en_index.dat"); */
    pdict->init("../data/dict_ch.dat", "../data/dict_en.dat", "../data/dictIndex_ch.dat", "../data/dictIndex_en.dat");

    vector<pair<string,int>> dict = pdict->getDict();
    /* for(auto & elem : dict) */
    /* { */
        /* cout << elem.first <<" " << elem.second << endl; */

        /* string str = pdict->doQuery(elem.first); */
        /* cout << str << endl; */
    /* } */
    for(size_t idx = 0; idx < 100; ++idx)
    {
        cout << dict[idx].first << " " << dict[idx].second << endl;
    }
    cout << endl;

    /* map<string, set<int>> indextable = pdict->getIndexRable(); */
    /* for(auto & it :indextable) */
    /* { */
    /*     cout << it.first << " "; */
    /*     for(auto & it2 : it.second) */
    /*     { */
    /*         cout << it2 << " ";; */
    /*     } */
    /*     cout << endl; */
    /* } */


    return 0;
}
#endif
