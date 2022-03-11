#pragma once
#include <pthread.h>
#include <semaphore.h>

class Mutex{
    private:
        pthread_mutex_t mutex;      //互斥锁

    public:
        Mutex();
        ~Mutex();
        void lock();                //上锁
        void unlock();              //解锁
        pthread_mutex_t* getmutex(); //获取互斥锁    
};

class Sem{
    private:
        sem_t sem;

    public:
        Sem();
        Sem(int);
        ~Sem();
        void wait();
        void post();
};

class Condition{
    private:
        pthread_cond_t cond;       //条件变量

    public:
        Condition();
        ~Condition();
        bool wait(pthread_mutex_t* mutex);
        void signal();
        void broadcast();


};