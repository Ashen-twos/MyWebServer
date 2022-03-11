#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <queue>
#include "PthreadPool.h"
/*
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond2 = PTHREAD_COND_INITIALIZER;

const int maxx = 10;
int tmp = 0;
std::queue<char> que;

void* producer(void* arg)
{
    while (1)
    {
        pthread_mutex_lock(&mutex);
        if(que.size()==maxx)
        {
            pthread_cond_wait(&cond2,&mutex);
        }
        tmp = (++tmp)%26;
        char now = 'a'+tmp;
        que.push(now);
        printf("%d put %c\n",(int)pthread_self(),now);
        pthread_mutex_unlock(&mutex);
        pthread_cond_signal(&cond);
        sleep(1+random()%3);
    }
    return NULL;
}

void* customer(void* arg)
{
    while (1)
    {
        pthread_mutex_lock(&mutex);
        if(que.empty())
        {
            pthread_cond_wait(&cond,&mutex);
        }
        printf("%d take %c\n",(int)pthread_self(),que.front());
        que.pop();
        sleep(1);
        pthread_mutex_unlock(&mutex);
        pthread_cond_signal(&cond2);
        sleep(8+random()%3);
    }
    
}
*/
void tasks(int a)
{

    printf("%d %d %d\n",a,a+1,a+2);

}


int main()
{/*
    pthread_t sd[10],pd[10];
    pthread_attr_t attr;
    int ret = pthread_attr_init(&attr);
    if(ret==0)
        printf("attr init success\n");
    pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
    
    


    for(int i=0; i<10; i++)
    {
        pthread_create(&pd[i],&attr,producer,NULL);
        pthread_create(&sd[i],&attr,customer,NULL);
    }
    
    getchar();
    pthread_attr_destroy(&attr);
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);*/
    
    PthreadPool* pool = new PthreadPool();
    while (1)
    {
        pool->push(std::bind(tasks,5));
        pool->push(std::bind(tasks,50));
        pool->push(std::bind(tasks,500));
        //sleep(5);
    }
    
    
}