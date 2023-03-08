#include"../include/KeyRecommander.h"
#include"../include/Dictionary.h"
#include<sstream>
#include<iostream>

using std::pair;
using std::stringstream;
using std::cout;
using std::endl;

/* Dictionary* Dictionary::_pInstance = nullptr; */
/* Dictionary::Autorelease Dictionary::_ar; */

/* KeyRecommander::KeyRecommander() */
/* { */
    /* Dictionary* pdict =  Dictionary::createInstance(); */
    /* pdict->init("../data/dict_cn.dat", "../data/dict_en.dat", "../data/indexCN.dat", "../data/indexEN.dat"); */
/* } */


string KeyRecommander::execute(const string & keys)
{
    _queryWord = keys;
    _finallyResult.clear();
    
    queryIndexTable();
    handleResult();
    
    string msg = response();

    return msg;
}

void KeyRecommander::queryIndexTable()
{
    Dictionary* pdict = Dictionary::getInstance();
    string result = pdict->doQuery(_queryWord);

    stringstream ss(result);
    string ch;
    while(ss >> ch)
    {
        set<int> index = (pdict->getIndexRable())[ch];
        statistic(index);    
    }

}

void KeyRecommander::statistic(set<int>& iset)
{
    for(auto& it : iset)
    {
        vector<pair<string, int>>& dict = Dictionary::getInstance()->getDict();
        Myresult myresult(dict[it].first, dict[it].second, distance(dict[it].first)); 
        _resultQue.push(myresult);
    }
}

//决定输出候选词的数量，然后输出到客户端
string KeyRecommander::response()
{
    /* cout << _finallyResult.size() << endl; */
    /* for(size_t idx = 0; idx < _finallyResult.size(); ++idx) */
    string str;

    for(size_t idx = 0; idx < 5 && idx < _finallyResult.size(); ++idx)
    {
        str += _finallyResult[idx];
        str += " ";
    }

    if(str.size() == 0){
        str = "没有相应的关键词";
    }

    return str;
}

void KeyRecommander::handleResult()
{
    map<string,int> tmp;
    /* size_t len = _resultQue.size(); */
    /* cout << "resultque.size: " << len << endl; */
    while(!_resultQue.empty())
    {
        string word = _resultQue.top()._word;
        if(tmp[word] != 0)
        {
            _resultQue.pop();
            continue;
        }
        tmp[word] = 1;
        _finallyResult.push_back(word);
        _resultQue.pop();
    }
    /* cout << "vector.size: " << _finallyResult.size() << endl; */
}

int KeyRecommander::distance(const string & rhs)
{//计算最小编辑距离-包括处理中英文
	size_t lhs_len = length(_queryWord);
	size_t rhs_len = length(rhs);
	int editDist[lhs_len + 1][rhs_len + 1];
	for(size_t idx = 0; idx <= lhs_len; ++idx)
	{
		editDist[idx][0] = idx;
	}

	for(size_t idx = 0; idx <= rhs_len; ++idx)
	{
		editDist[0][idx] = idx;
	}
	
	std::string sublhs, subrhs;
	for(std::size_t dist_i = 1, lhs_idx = 0; dist_i <= lhs_len; ++dist_i, ++lhs_idx)
	{
		size_t nBytes = nBytesCode(_queryWord[lhs_idx]);
		sublhs = _queryWord.substr(lhs_idx, nBytes);
		lhs_idx += (nBytes - 1);

		for(std::size_t dist_j = 1, rhs_idx = 0; dist_j <= rhs_len; ++dist_j, ++rhs_idx)
		{
			nBytes = nBytesCode(rhs[rhs_idx]);
			subrhs = rhs.substr(rhs_idx, nBytes);
			rhs_idx += (nBytes - 1);
			if(sublhs == subrhs)
			{
				editDist[dist_i][dist_j] = editDist[dist_i - 1][dist_j - 1];
			}
			else
			{
				editDist[dist_i][dist_j] = triple_min(
					editDist[dist_i][dist_j - 1] + 1,
					editDist[dist_i - 1][dist_j] + 1,
					editDist[dist_i - 1][dist_j - 1] + 1);
			}
		}
	}
	return editDist[lhs_len][rhs_len];
}

size_t KeyRecommander::nBytesCode(const char ch)
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

size_t KeyRecommander::length(const std::string &str)
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

int KeyRecommander::triple_min(const int &a, const int &b, const int &c)
{
	return a < b ? (a < c ? a : c) : (b < c ? b : c);
}

//test
#if 0
int main()
{
    Dictionary* pdict =  Dictionary::createInstance();
    /* pdict->init("../file/en_dict.dat", "../file/en_index.dat"); */
    pdict->init("../data/dict_ch.dat", "../data/dict_en.dat", "../data/dictIndex_ch.dat", "../data/dictIndex_en.dat");
    string s1("hello");
    string s2("world");
    string s3("北京师范大学hello");
    KeyRecommander keyWord(s3);
    keyWord.execute();


    return 0;
}

#endif
