#include "MySQLPool.h"

MySQLPool::MySQLPool()
{

}

MySQLPool::~MySQLPool()
{

}

MySQLPool* MySQLPool::GetInstance()
{
    static MySQLPool ret;
    return &ret;
}

void MySQLPool::init(const char* host, int port, const char* dbname,
                     const char* user, const char* password, int cap)
{
    //初始化数据库信息
    sql_host = host;
    sql_port = port;
    sql_dbname = dbname;
    sql_user = user;
    sql_password = password;
    capacity = cap;

    //创建数据库连接
    for(int i=0; i<capacity; i++)
    {
        MYSQL *con = nullptr;
        con = mysql_init(con);
        if(!con)
        {
            LOG_ERROR("SQL Init Error1");
            throw std::exception();
        }
        con = mysql_real_connect(con,host,user,password,dbname,port,NULL,0);
        if(!con)
        {
            LOG_ERROR("SQL Init Error2");
            throw std::exception();
        }
        pool.push(con);
    }
    //printf("Pool size:%d\n",pool.size());
    sem = Sem(pool.size());
    LOG_INFO("SQL Init Success");
}

MYSQL* MySQLPool::GetConnection()
{
    MYSQL* con = NULL;
    /*
    if(pool.empty())
        return con;
    */
    sem.wait();
    mutex.lock();

    con = pool.front();
    pool.pop();

    mutex.unlock();
    return con;
}

bool MySQLPool::ReturnConnection(MYSQL* con)
{
    if(!con)
        return false;
    mutex.lock();

    pool.push(con);
    
    mutex.unlock();
    sem.post();
    return true;
}

void MySQLPool::Close()
{
    mutex.lock();
    while(!pool.empty())
    {
        mysql_close(pool.front());
        pool.pop();
    }
    mutex.unlock();
}

bool MySQLPool::empty()
{
    return pool.size();
}



