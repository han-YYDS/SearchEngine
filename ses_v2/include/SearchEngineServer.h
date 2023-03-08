#ifndef __SEARCHENGINESERVER_H__
#define __SEARCHENGINESERVER_H__

#include "TcpServer.h"
#include "ThreadPool.h"
#include "KeyRecommander.h"
#include "WebPageSearcher.h"
#include "ProtocolParser.h"
#include "Dictionary.h"
#include "Mylogger.h"
#include "MyRedis.h"
#include "Configuration.h"

#include <iostream>
#include <unistd.h>

#include <nlohmann/json.hpp>

using std::cout;
using std::endl;

using json = nlohmann::json;

Dictionary* Dictionary::_pInstance = nullptr;
Dictionary::Autorelease Dictionary::_ar;


class SearchEngineServer
{
public:
    // 构造函数
    SearchEngineServer(const string & ip, short port)
    : _pool(3, 10) 
    ,_tcpServer(ip, port)
    {

    }

    // 开始提供服务
    void start()
    {
        // 加载词典库文件
        Configuration *config = Configuration::getInstance();
        map<string, string> configMap = config->getConfigMap();
        string dict_cn_path = configMap["dict_cn.dat"];
        string dict_en_path = configMap["dict_en.dat"];
        string indexCN_path = configMap["indexCN.dat"];
        string indexEN_path = configMap["indexEN.dat"];
        
        Dictionary* pdict =  Dictionary::getInstance();
        pdict->init(dict_cn_path, dict_en_path, indexCN_path, indexEN_path);

        // 连接Redis 
        _myredis.connect("127.0.0.1", 6379);

        // 启动线程池
        _pool.start();
        
        // 开启网络服务
        using namespace std::placeholders;
        _tcpServer.setAllCallback( std::bind(&SearchEngineServer::onConnection, this, _1)
                                  ,std::bind(&SearchEngineServer::onMessage, this, _1)
                                  ,std::bind(&SearchEngineServer::onClose, this, _1));
        _tcpServer.start();
    
    }

    // 停止提供服务
    void stop()
    {
        _pool.stop();
        _tcpServer.stop();
    }

private:
    // 被注册回调函数，提供给 TcpServer 使用
    void onConnection(const TcpConnectionPtr &con)
    {
        string interface = "-------------------用户交互界面-------------------\n1. 关键字查询\n2. 网页查询\n\n请输入'1'或'2': ";
        con->sendInLoop(interface, 0);
        cout << con->toString() << " has connected!" << endl;
    }
    
    // 被注册回调函数，提供给 TcpServer 使用
    void onMessage(const TcpConnectionPtr &con)
    {
        //实现了搜索引擎服务
        string msg =  con->receive();
        cout << "recv from client msg :\n" << msg << endl;
        
        // 执行查询
        _pool.addTask(std::bind(&SearchEngineServer::doTaskThread, this, con, msg));

    }

    // 被注册回调函数，提供给 TcpServer 使用
    void onClose(const TcpConnectionPtr &con)
    {
        cout << con->toString() << " has closed!" << endl;
    }

    // 该方法由线程池的某一个线程执行
    void doTaskThread(const TcpConnectionPtr & conn, const string & msg)
    {
        LogInfo(msg);

        // 解析msg
        _protocolParser.doParse(msg);
        int msgId = _protocolParser.getMsgId();
        string query = _protocolParser.getMsgBody();
        
        cout << "msgId: " << msgId << endl;                            
        cout << "msgBody: " << query << endl;

        string res;
        if(msgId == 1){
            LogInfo("执行关键字推荐");
        
            // 执行关键字推荐
            _myredis.select("0");
            if(_myredis.exists(query)){
                // 从缓存中获取
                res = _myredis.get(query);

            }else{
                res =_keyRecommander.execute(query);
                
                // 加入缓存
                _myredis.set(query, res);

            }
            
            json jRes;
            jRes["queryWords"] = query;
            jRes["results"] = res;
            
            res = jRes.dump();

            conn->sendInLoop(res, 100);

        }else if(msgId == 2){
            LogInfo("执行网页查询");
           
            // 网页查询
            _myredis.select("1");
            if(_myredis.exists(query)){
                // 从缓存中获取
                res = _myredis.get(query);

            }else{
                res = _webSearcher.doQuery(query);
                
                // 加入缓存
                _myredis.set(query, res);
        
            }
        
            conn->sendInLoop(res, 200);
        
        }
    }

private:
    ThreadPool _pool;     //线程池
    TcpServer _tcpServer;       // TCP 通信模块
    KeyRecommander _keyRecommander;     //模块一：关键字推荐
    WebPageSearcher _webSearcher;       //模块二：网页查询
    ProtocolParser _protocolParser;     //协议解析模块
    MyRedis _myredis;
};

#endif
