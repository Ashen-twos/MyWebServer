#pragma once

#include <stdio.h>
#include <queue>
#include "../pthread/PthreadSync.h"

template<typename T>
class BlockQue{
    public:
        BlockQue(int size = 1024);
        ~BlockQue();

    private:
        std::queue<T> que;      //队列
        int max_size;           //最大容量
        Mutex mutex;            //互斥锁
        Condition cond;         //条件变量

    //接口
    public:
        int size();             //队列中元素数量
        int capacity();         //队列最大容量
        bool empty();           //队列是否为空
        bool full();            //队列是否已满
        bool front(T& item);    //获得队首元素
        bool push(T& item);     //入队
        bool pop(T& item);      //出队
        void clear();           //清空队列
};

template<typename T>
BlockQue<T>::BlockQue(int size) :
max_size(size), mutex(), cond()
{

}

template<typename T>
BlockQue<T>::~BlockQue()
{
    mutex.lock();
    while(!que.empty())
        que.pop();
    mutex.unlock();
}

template<typename T>
int BlockQue<T>::size()
{
    int ret = -1;
    mutex.lock();
    ret = que.size();
    mutex.unlock();
    return ret;
}

template<typename T>
int BlockQue<T>::capacity()
{
    return max_size;
}

template<typename T>
bool BlockQue<T>::empty()
{
    bool ret = false;
    mutex.lock();
    ret = que.empty();
    mutex.unlock();
    return ret;
}

template<typename T>
bool BlockQue<T>::full()
{
    ////printf("Start check full\n");
    bool ret = false;
    mutex.lock();
    ret = (que.size()>=max_size);
    mutex.unlock();
    return ret;
}

template<typename T>
bool BlockQue<T>::front(T& item)
{
    mutex.lock();
    item = que.front();
    mutex.unlock();
    return true;
}

template<typename T>
bool BlockQue<T>::push(T& item)
{
    //printf("Start push\n");
    mutex.lock();
    if(que.size()>=max_size)
    {
        cond.broadcast();
        mutex.unlock();
        return false;
    }
    que.push(item);
    cond.broadcast();
    mutex.unlock();
    return true;
}

template<typename T>
bool BlockQue<T>::pop(T& item)
{
    mutex.lock();
    while(que.empty())
    {
        if(!cond.wait(mutex.getmutex()))
        {
            mutex.unlock();
            return false;
        }
    }
    item = que.front();
    que.pop();
    mutex.unlock();
    return true;
}

template<typename T>
void BlockQue<T>::clear()
{
    mutex.lock();
    que.clear();
    mutex.unlock();
}