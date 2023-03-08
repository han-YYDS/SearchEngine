#include "Client.h"

#include <iostream>
#include <string>

using std::string;

int main(int argc, char *argv[])
{
    printf("\033[;47;32m你好，欢迎使用！\033[0m\n");
    
    string ip = "127.0.0.1";
    int port = 8888;

    Client client(ip, port);

    client.clientLoop();

    return 0;
}
