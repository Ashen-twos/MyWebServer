#include "Timer.h"

TimerNode::TimerNode() :
isdelete(false), expiredtime(0)
{

}

TimerNode::TimerNode(std::shared_ptr<HttpData> httpdata, int time, const std::function<void()>& callback)
{
    init(httpdata,time,callback);
}

void TimerNode::init(std::shared_ptr<HttpData> httpdata, int time, const std::function<void()> callback)
{
    struct timeval now;
    gettimeofday(&now, NULL);
    expiredtime = (((now.tv_sec % 10000) * 1000) + (now.tv_usec / 1000)) + time;
    cb = callback;
    isdelete = false;
    LOG_INFO("Timer Init Success");
}       

size_t TimerNode::GetExpiretime()
{
    return expiredtime;
}

void TimerNode::update(int time)
{
    expiredtime += time;
}

bool TimerNode::IsDelete()
{
    if(isdelete)
        return isdelete;
    struct timeval now;
    gettimeofday(&now,NULL);
    size_t temp = (((now.tv_sec % 10000) * 1000) + (now.tv_usec / 1000));
    if(temp < expiredtime)
        return false;
    else
    {
        isdelete = true;
        return true;
    }
}

TimerNode::~TimerNode()
{

}

TimerManager::TimerManager()
{

}

TimerManager::~TimerManager()
{

}

void TimerManager::pushtimer(std::shared_ptr<HttpData> data, int time, const std::function<void()> cb)
{
    if(alive)
    {
        data->timer->init(data,time,cb);
         TimerQue.push(data->timer);
    }
}

void TimerManager::update()
{
    while(alive && !TimerQue.empty())
    {
        if(TimerQue.top()->IsDelete())
        {
            TimerQue.top()->cb();
            TimerQue.pop();
        }
        else
            break;
    }
}

void TimerManager::setalive(bool isalive)
{
    alive = isalive;
}