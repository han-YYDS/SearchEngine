// #define _GLIBCXX_USE_CXX11_ABI=0
#ifndef __CONFIGURATION_H__
#define __CONFIGURATION_H__

#include <string>
#include <map>
#include <set>

using std::string;
using std::map;
using std::set;

/*
    配置文件类 - 含内部释放类的单例类

    在define处修改 配置文件路径 和 stoplistname

    具体使用见conf_test文件

*/

class Configuration
{
class AutoRelease
{
public:
    AutoRelease()
    {}
    ~AutoRelease()
    {
        if(_pInstance) {
            delete _pInstance;
        }
    }
};
public:
    static Configuration * getInstance()
    {
        if(_pInstance == nullptr) {
            _pInstance = new Configuration();
        }
        return _pInstance;
    }

	void init(string filepath)
	{	
        _filepath = filepath;
        genConfigMap();
        genStopWordList();
	}

    set<string> getStopWordList();   // 获取停用词集

    map<string, string> & getConfigMap(); // 获取配置文件map


private:
	Configuration() {}
	~Configuration() {}

private:
	static Configuration * _pInstance;
	static AutoRelease _ar;

    string _filepath;               // 配置文件路径
    map<string,string> _configMap;  //  <.dat文件 ： 文件路径>
    set<string> _stopWordList;      //  停用词集

    void genConfigMap();  // 生成配置文件
    void genStopWordList();      // 生成停用词



};


#endif
