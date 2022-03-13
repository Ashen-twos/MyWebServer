#pragma once
#include "HttpRequest.h"
#include "HttpMsg.h"
#include <memory>
#include "../Package/Epoll.h"
#include "../Package/InetAddress.h"
#include "../Package/Socket.h"
#include "../Package/util.h"
#include "../Timer/Timer.h"

class TimerNode;

class HttpData{
    public:
        static int epfd;            //epollfd
        std::shared_ptr<HttpRequest> request;       //请求
        std::shared_ptr<HttpMsg> message;           //应答  
        std::shared_ptr<TimerNode> timer;           //对应的计时器
        int sockfd;                 //用户socket
        bool alive;



    public:
        HttpData();
        HttpData(int, int);
        ~HttpData();
        void init();

    public:
        int read(int *);         //单次读数据
        int write(int *);        //单次写数据
        void disconnect();       //断开连接
        void process();          //执行
        bool parse_and_make();
        







    

};

