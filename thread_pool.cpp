#include <iostream>
#include <pthread.h>
#include "thread_pool.h"

using namespace std;

pthread_mutex_t* mutex_ptr;

thread_pool::thread_pool()
{
    pthread_mutex_init(&mutex, nullptr);
    mutex_ptr = &mutex;

    for(int i=0;i<MAX_THREAD;i++)
    {
        init_arg[i].order = i;
        init_arg[i].pool = this;
        pthread_create(&init_arg[i].tid, nullptr, thread_loop, &init_arg[i]);
    }

}

thread_pool::~thread_pool()
{
    for(int i=0;i<MAX_THREAD;i++)
    {
        while(1)
        {
            pthread_mutex_lock(&mutex);
            if(thread_list[i].tag == 0)
            {
                thread_list[i].task = thread_exit;
                thread_list[i].tag = 1;
                pthread_mutex_unlock(&mutex);
                break;
            }
            pthread_mutex_unlock(&mutex);
        }
    }

    for(int i=0;i<MAX_THREAD;i++)
    {
        pthread_join(init_arg[i].tid, NULL);
    }
}

int thread_pool::run(void* (*task)(void *), void *arg)
{
    for(int i=0;i<MAX_THREAD;i++)
    {
        pthread_mutex_lock(&mutex);
        if(thread_list[i].tag == 0)
        {
            thread_list[i].task = task;
            thread_list[i].arg = arg;
            thread_list[i].tag = 1;
            pthread_mutex_unlock(&mutex);
            count++;
            return 0;
        }
        pthread_mutex_unlock(&mutex);
    }
    getchar();
    return -1;
}

void *thread_loop(void * arg)
{
    struct thread_init* thi = (struct thread_init*)arg;

    while(1)
    {
        if(thi->pool->thread_list[thi->order].tag == 0)
        {

        }
        else
        {
            thi->pool->thread_list[thi->order].task(thi->pool->thread_list[thi->order].arg);
            pthread_mutex_lock(&thi->pool->mutex);
            thi->pool->thread_list[thi->order].task = nullptr;
            thi->pool->thread_list[thi->order].tag = 0;
            pthread_mutex_unlock(&thi->pool->mutex);
            thi->pool->count--;
        }
    }
}

void *thread_exit(void *)
{
    pthread_exit(0);
    return nullptr;
}
