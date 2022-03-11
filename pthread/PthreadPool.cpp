#include "PthreadPool.h"

PthreadPool::PthreadPool(int max_poolsize, int max_quesize) : 
poolsize(max_poolsize), quesize(max_quesize), workstate(true), p_sem(), p_mutex()
{
    //线程池参数不合法
    if(max_poolsize<=0 || max_quesize<=0)           
        throw std::exception();
    pool = new pthread_t[max_poolsize];
    //线程池创建失败
    if(!pool)       
        throw std::exception();
    
    //初始化线程
    for(int i=0; i<max_poolsize; i++)
    {
        if(pthread_create(pool+i,NULL,work,this))
        {
            delete [] pool;
            throw std::exception();
        }
        //设置分离状态
        if(pthread_detach(pool[i]))
        {
            delete [] pool;
            throw std::exception();
        }
    }
}

PthreadPool::~PthreadPool()
{
    workstate = false;
    delete [] pool;
}

void* PthreadPool::work(void*arg)
{
    PthreadPool* p = (PthreadPool*) arg;
    p -> run();
    return p;
}


void PthreadPool::run()
{
    while (workstate)
    {
        p_sem.wait();           //抢占任务
        p_mutex.lock();
        if(taskqueue.empty())   
        {
            p_mutex.unlock();
            continue;
        }
        std::function<void()> task = std::move(taskqueue.front());
        taskqueue.pop();
        p_mutex.unlock();
        //执行任务
        task();
        
    }
    
}