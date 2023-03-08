#include "ProtocolParser.h"

#include <string>

using std::string;

#define NETBUFSIZE 5000

typedef struct  {
    int length;
    int msgId;
    char data[NETBUFSIZE];
} train_t;

class Client
{
public:
    Client(string ip, int port)
    :_ip(ip)
    ,_port(port)
    {

    }

    void clientLoop();

    int sendn(int fd, const void * buf, int len);
    int sendMessage(int peerfd, string msg, int msgId);

private:
    string _ip;
    int _port;
    ProtocolParser _parser;

};
