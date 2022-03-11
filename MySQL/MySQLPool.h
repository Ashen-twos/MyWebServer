#pragma once

#include <string>
#include <queue>
#include <mysql/mysql.h>
#include "../Log/Log.h"
#include "../pthread/PthreadSync.h"

class MySQLPool{
    public:
        static MySQLPool* GetInstance();            //单例模式
        void init(const char* host, int port, const char* dbname,
                  const char*user, const char* password, int cap);                                //初始化连接池
        MYSQL* GetConnection();                     //获取连接          
        bool ReturnConnection(MYSQL* con);          //归还连接
        void Close();                               //关闭数据库连接池
        bool empty();                               //当前连接池是否为空

    private:
        MySQLPool();
        ~MySQLPool();

    private:
        std::queue<MYSQL*> pool;       //连接池
        int capacity;                   //容量
        Mutex mutex;                    //互斥锁
        Sem sem;                        //信号量


        //数据库信息
        std::string sql_host;           //主机号
        int sql_port;                   //端口号
        std::string sql_dbname;         //数据库名称
        std::string sql_user;           //数据库用户名
        std::string sql_password;       //数据库密码
};

