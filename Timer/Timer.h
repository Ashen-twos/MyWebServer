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
        TimerNode(std::shared_ptr<HttpData> httpdata, int time, const std::function<void()>& callback);
        ~TimerNode();
    
    public:
        size_t GetExpiretime();
        bool IsDelete();
        std::function<void()> cb;   //回调函数
        void init(std::shared_ptr<HttpData> httpdata, int time, const std::function<void()> callback);  
        void update(int);              
        
    
    private:
        
        bool isdelete;              //是否过期
        size_t expiredtime;         //持续时间
        
};

//仿函数,用于比较节点指针
struct cmp
{
    bool operator()(std::shared_ptr<TimerNode> a, std::shared_ptr<TimerNode> b)const
    {
        return a->GetExpiretime() > b->GetExpiretime();
    }
};


class TimerManager{
    public:
        TimerManager();
        ~TimerManager();

    public:
        void pushtimer(std::shared_ptr<HttpData> httpdata, int time, const std::function<void()> cb);
        void update();
        void setalive(bool);

    private:
        std::priority_queue<std::shared_ptr<TimerNode>,std::deque<std::shared_ptr<TimerNode>>,cmp> TimerQue;
        bool alive;


};