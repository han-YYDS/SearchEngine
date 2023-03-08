#ifndef __Dictionary_H__
#define __Dictionary_H__
#include<vector>
#include<string>
#include<map>
#include<set>


using std::vector;
using std::string;
using std::pair;
using std::map;
using std::set;


class Dictionary
{
    class Autorelease
    {
    public:
        Autorelease()
        {
        }
        ~Autorelease()
        {
            if(_pInstance)
            {
                delete _pInstance;
                _pInstance = nullptr;
            }
        }
    };

public:
    static Dictionary* getInstance()
    {
        if(nullptr == _pInstance)
        {
            _pInstance = new Dictionary();
            _ar;
        }
        return _pInstance;
    }

    void init(const string & cn_dictpath,const string & en_dictpath, const string& cn_indexpath,const string& en_indexpath);
    vector<pair<string, int>>& getDict();
    map<string, set<int>> & getIndexRable();
    string doQuery(const string & word);
    size_t nBytesCode(const char ch);
    size_t length(const std::string &str);

private:
    vector<pair<string, int>> _dict;
    map<string, set<int>> _indexTable;
    static Dictionary* _pInstance;
    static Autorelease _ar;
};


#endif
