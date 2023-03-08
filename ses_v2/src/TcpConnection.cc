#include "../include/TcpConnection.h"
#include "../include/EventLoop.h"
#include <iostream>
#include <sstream>

#include <func.h>
#include <nlohmann/json.hpp>

using std::cout;
using std::endl;
using std::ostringstream;
using std::to_string;

using json = nlohmann::json;

TcpConnection::TcpConnection(int fd, EventLoop *loop)
: _loop(loop)
, _sock(fd)
, _sockIO(fd)
, _localAddr(getLocalAddr())
, _peerAddr(getPeerAddr())

{

}

TcpConnection::~TcpConnection()
{

}

void TcpConnection::send(const string &msg, int msgId)
{
    train_t train;
 
    memset(train.data, '\0', sizeof(train.data));                                                                   
    strcpy(train.data, msg.data());
    train.length = strlen(train.data);                     
    train.msgId = msgId;
    _sockIO.writen(&train, 8+train.length);
}

void TcpConnection::sendInLoop(const string &msg, int msgId)
{
    //消息msg需要从TcpConnection发送给EventLoop
    /* this->send(msg); */
    if(_loop)
    {
        _loop->runInLoop(std::bind(&TcpConnection::send, this, msg, msgId));
        //void runInLoop(function<void()>)
    }
}

string TcpConnection::receive()
{
    char buff[65535] = {0};
    /* _sockIO.readLine(buff, sizeof(buff)); */
    int msgId = _sockIO.readTrain(buff, sizeof(buff));

    string msgBody = string(buff);
    
    json msg;
    msg["msgId"] = to_string(msgId);
    msg["msgBody"] = msgBody;
    string res = msg.dump();

    return res;
}

string TcpConnection::toString()
{
    ostringstream oss;
    oss << _localAddr.ip() << ":"
        << _localAddr.port() << "---->"
        << _peerAddr.ip() << ":"
        << _peerAddr.port();

    return oss.str();
}

bool TcpConnection::isClosed() const
{
    char buf[10] = {0};
    int ret = recv(_sock.fd(), buf, sizeof(buf), MSG_PEEK);

    return ret == 0;
}

void TcpConnection::setConnectionCallback(const TcpConnectionCallback &cb)
{
    _onConectionCb = cb;
}

void TcpConnection::setMessageCallback(const TcpConnectionCallback &cb)
{
    _onMessageCb = cb;
}

void TcpConnection::setCloseCallback(const TcpConnectionCallback &cb)
{
    _onCloseCb = cb;
}

void TcpConnection::handleConnectionCallback()
{
    //this ->TcpConnection > (this - >shared_ptr)
    if(_onConectionCb)
    {
        /* _onConectionCb(shared_ptr<TcpConnection>(this)); */
        _onConectionCb(shared_from_this());
    }
}
void TcpConnection::handleMessageCallback()
{
    if(_onMessageCb)
    {
        _onMessageCb(shared_from_this());
    }

}
void TcpConnection::handleCloseCallback()
{
    if(_onCloseCb)
    {
        _onCloseCb(shared_from_this());
    }

}

//获取本端的网络地址信息
InetAddress TcpConnection::getLocalAddr()
{
    struct sockaddr_in addr;
    socklen_t len = sizeof(struct sockaddr );
    int ret = getsockname(_sock.fd(), (struct sockaddr *)&addr, &len);
    if(-1 == ret)
    {
        perror("getsockname");
    }

    return InetAddress(addr);

}

//获取对端的网络地址信息
InetAddress TcpConnection::getPeerAddr()
{
    struct sockaddr_in addr;
    socklen_t len = sizeof(struct sockaddr );
    int ret = getpeername(_sock.fd(), (struct sockaddr *)&addr, &len);
    if(-1 == ret)
    {
        perror("getpeername");
    }

    return InetAddress(addr);

}
