#include "Client.h"

#include <func.h>
#include <stdlib.h>

#include <nlohmann/json.hpp>
#include <iostream>

using std::cout;
using std::endl;

using json = nlohmann::json;

void Client::clientLoop()
{
	// 创建套接字
	int clientfd =  socket(AF_INET, SOCK_STREAM, 0);
	ERROR_CHECK(clientfd, -1, "socket");

	struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(_port);
	serveraddr.sin_addr.s_addr = inet_addr(_ip.data());

	// 向server服务器发起建立连接的请求
	int ret;
    ret = connect(clientfd, 
			(struct sockaddr *)&serveraddr, 
			sizeof(serveraddr));
	ERROR_CHECK(ret, -1, "connect");
	cout << "main thread: connect has completed." << endl;
    
    fd_set readfds;
    
    int length;
    int msgId;
    char buff[NETBUFSIZE];
    while(1){
        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds);
        FD_SET(clientfd, &readfds);
        
        select(clientfd + 1, &readfds, NULL, NULL, NULL);
            
        if(FD_ISSET(clientfd, &readfds)){
            int retNet;
            retNet = recv(clientfd, &length, 4, MSG_WAITALL);
            retNet = recv(clientfd, &msgId, 4, MSG_WAITALL);
            
            printf("\n\033[;47;32mMsgLen:%5d, MsgID:%5d\033[0m\n", length, msgId);

            if(msgId == 0){          // 普通消息
                memset(buff, '\0', sizeof(buff));
	            ret = recv(clientfd, buff, length, MSG_DONTWAIT);
                
	            cout << buff << std::flush;

            }if(msgId == 100){          // 推荐的相关关键字
                memset(buff, '\0', sizeof(buff));
	            ret = recv(clientfd, buff, length, MSG_DONTWAIT);
                
                _parser.doParse(buff);
	            cout << "查询词：" << _parser.getQueryWords() << endl; 
                cout << "查询结果: " << endl;
                cout << _parser.getResults() << endl;
            
                string interface = "\n-------------------用户交互界面-------------------\n1. 关键字查询\n2. 网页查询\n\n请输入'1'或'2': ";
                cout << interface << std::flush;
            
            }else if(msgId == 200){   // 关键字相关网页信息
                memset(buff, '\0', sizeof(buff));
	            ret = recv(clientfd, buff, length, MSG_DONTWAIT);

                _parser.doParse(buff);
	            cout << "查询词：" << _parser.getQueryWords() << endl; 
                cout << "查询结果: " << endl;
                cout << _parser.getResults() << endl;
                
                string interface = "\n-------------------用户交互界面-------------------\n1. 关键字查询\n2. 网页查询\n\n请输入'1'或'2': ";
                cout << interface << std::flush;

            }
            
            if(retNet == 0 ){
                // 服务器server断开连接
                cout << "clientfd" << clientfd <<" was broken by server..." << endl;
                break;
            }

        }
            
        if(FD_ISSET(STDIN_FILENO, &readfds)){
            memset(buff, '\0', sizeof(buff));
            read(STDIN_FILENO, buff, sizeof(buff));
            /* cout <<"buff: " << buff << endl; */
            
            if(strcmp(buff, "\n") != 0){
                if(strcmp(buff, "1\n") == 0)
                {
                    msgId = 1;
                    cout << "请输入要查询的内容: " << std::flush;

                }else if (strcmp(buff, "2\n") == 0)
                {
                    msgId = 2;
                    cout << "请输入要查询的内容: " << std::flush;
                }else{
                    if(msgId == 1){
                        buff[strlen(buff)-1] = '\0';
                        ret = sendMessage(clientfd, buff, 1);

                    }else if(msgId == 2){
                        buff[strlen(buff)-1] = '\0';
                        ret = sendMessage(clientfd, buff, 2);

                    }else{
                        cout << "输入错误!\n请输入'1'或'2': " << std::flush;

                    }
                }
            }else{
                cout << "> " << std::flush;
            }
        }
    }
   
	close(clientfd);
}

int Client::sendn(int fd, const void * buf, int len)
{
    int left = len;
    char * pbuf = (char *)buf;
    int ret;
    while(left > 0) {
        ret = send(fd, pbuf, left, 0);
        if(ret < 0) {
            perror("send");
            break;
        } else {
            pbuf += ret;
            left -= ret;
        }
    }
    return len - left;
}

int Client::sendMessage(int peerfd, string msg, int msgId )
{
    train_t train;

    memset(train.data, '\0', sizeof(train.data));
    strcpy(train.data, msg.data());
    train.length = strlen(train.data);
    train.msgId = msgId;
    sendn(peerfd, &train, 8+train.length);

    return 1;
}

