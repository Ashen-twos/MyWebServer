#pragma once
#include <functional>
#include <pthread.h>
#include "PthreadSync.h"
#include <queue>

class PthreadPool{
    private:
        pthread_t *pool;            //线程池
        int poolsize;               //线程池容量
        std::queue<std::function<void()>> taskqueue;   //任务队列
        int quesize;                //队列容量
        Mutex p_mutex;              //互斥锁
        Sem p_sem;                  //信号量
        bool workstate;             //工作状态

    public:
        PthreadPool(int max_poolsize=8, int max_quesize=1024);
        ~PthreadPool();

        template<typename T>
        bool push(T&& ta)               //任务入队
        {
            p_mutex.lock();
            if(taskqueue.size() >= poolsize)
            {
                p_mutex.unlock();
                return false;
            }
            taskqueue.emplace(std::forward<T>(ta));
            p_mutex.unlock();
            p_sem.post();
            return true;
        }
    
    private:
        static void* work(void *arg);            //启动工作线程        
        void run();                       //开始工作
};