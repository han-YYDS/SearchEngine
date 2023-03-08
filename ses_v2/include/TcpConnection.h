#ifndef __TCPCONNECTION_H__
#define __TCPCONNECTION_H__

#include "Socket.h"
#include "SocketIO.h"
#include "InetAddress.h"

#include <memory>
#include <functional>

using std::shared_ptr;
using std::function;

#define NETBUFSIZE 5000

typedef struct  {
    int length;
    int msgId;
    char data[NETBUFSIZE];
} train_t;

class EventLoop;//前向声明

class TcpConnection
: public std::enable_shared_from_this<TcpConnection>
{
    using TcpConnectionPtr = shared_ptr<TcpConnection>;
    using TcpConnectionCallback = function<void(const TcpConnectionPtr &)>;
public:
    TcpConnection(int fd, EventLoop *loop);
    ~TcpConnection();
    void send(const string &msg, int msgId);
    void sendInLoop(const string &msg, int msgId);
    string receive();
    string toString();

    bool isClosed() const;

    void setConnectionCallback(const TcpConnectionCallback &cb);
    void setMessageCallback(const TcpConnectionCallback &cb);
    void setCloseCallback(const TcpConnectionCallback &cb);

    void handleConnectionCallback();
    void handleMessageCallback();
    void handleCloseCallback();

private:
    InetAddress getLocalAddr();
    InetAddress getPeerAddr();
private:
    EventLoop *_loop;
    Socket _sock;
    SocketIO _sockIO;
    InetAddress _localAddr;
    InetAddress _peerAddr;

    TcpConnectionCallback _onConectionCb;
    TcpConnectionCallback _onMessageCb;
    TcpConnectionCallback _onCloseCb;


};

#endif
