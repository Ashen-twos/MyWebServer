#pragma once

#include "../http/HttpData.h"
#include "../Log/Log.h"
#include <sys/time.h>
#include <memory>
#include <queue>
#include <functional>

class HttpData;

//计时器节点
class TimerNode{
    public:
        TimerNode();
        TimerNode(HttpData* httpdata, int time, const std::function<void()>& callback);
        ~TimerNode();
    
    public:
        size_t GetExpiretime();
        bool IsDelete();
        std::function<void()> cb;   //回调函数
        void init(HttpData* httpdata, int time, const std::function<void()> callback);  
        void update(int);              
        HttpData* data;             //对应连接的指针
        
    
    private:
        
        bool isdelete;              //是否过期
        size_t expiredtime;         //持续时间
        
};

//仿函数,用于比较节点指针
struct cmp
{
    bool operator()(TimerNode* a, TimerNode* b)const
    {
        return a->GetExpiretime() > b->GetExpiretime();
    }
};


class TimerManager{
    public:
        TimerManager();
        ~TimerManager();

    public:
        void pushtimer(HttpData* data, int time, const std::function<void()> cb);
        void update();
        void setalive(bool);

    private:
        std::priority_queue<TimerNode*,std::deque<TimerNode*>,cmp> TimerQue;
        bool alive;


};