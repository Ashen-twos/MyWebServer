#include "PthreadSync.h"
#include <exception>
#include<stdio.h>

Mutex::Mutex()
{
    if(pthread_mutex_init(&mutex,NULL))
        throw std::exception();
}

Mutex::~Mutex()
{
    pthread_mutex_destroy(&mutex);
}

void Mutex::lock()
{
    if(pthread_mutex_lock(&mutex))
    {
        printf("LOCK error\n");
        throw std::exception();
    }
}

void Mutex::unlock()
{
    if(pthread_mutex_unlock(&mutex))
        throw std::exception();
}

pthread_mutex_t* Mutex::getmutex()
{
    return &mutex;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

Sem::Sem()
{
    if(sem_init(&sem,0,0))
        throw std::exception();
}

Sem::Sem(int num)
{
    if(sem_init(&sem,0,num))
        throw std::exception();
}

Sem::~Sem()
{
    sem_destroy(&sem);
}

void Sem::wait()
{
    if(sem_wait(&sem))
       throw std::exception();
}

void Sem::post()
{
    if(sem_post(&sem))
        throw std::exception();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

Condition::Condition()
{
    if(pthread_cond_init(&cond,NULL))
        throw std::exception();
}

Condition::~Condition()
{
    pthread_cond_destroy(&cond);
}

bool Condition::wait(pthread_mutex_t* mut)
{
    return pthread_cond_wait(&cond,mut) == 0;
}

void Condition::signal()
{
    if(pthread_cond_signal(&cond))
        throw std::exception();
}

void Condition::broadcast()
{
    if(pthread_cond_broadcast(&cond))
        throw std::exception();
}
