#pragma once
#include "http/HttpData.h"
#include "pthread/threadpool.h"
#include "Timer/Timer.h"
#include "Log/Log.h"
#include "MySQL/MySQLPool.h"

#define MAX_CONN 65536

class Server
{
    public:
        char *root;           //运行目录
    
    private:
        Epoll ep;                   //epoll
        Socket sock;                //监听sock
        bool server_state;          //服务器状态
        ThreadPool pool;            //线程池
        InetAddress addr;           //服务器地址
        InetAddress client_addr;    //新用户地址
        int client_sock;            //用户sock 
        HttpData *conn;             //连接
        TimerManager timermanager;  //管理计时器
        MySQLPool* sql;             //数据库连接池

    private:
        bool acceptclient(int sockfd);               //接收连接

    public:
        Server();
        Server(const char* ip, int port, bool log, bool timer);
        ~Server();
        void init();          //服务器初始化     
        void evenloop();      //循环监听
        void run();           //服务器启动

        void OnRead(HttpData*);
        void OnWrite(HttpData*);
        void CloseConn(HttpData*);
        void initlog(bool log);
        

};