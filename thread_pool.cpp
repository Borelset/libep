#include <iostream>
#include <signal.h>
#include "thread_pool.h"

using namespace std;


thread_info::thread_info()
{
    task = nullptr;
    arg = nullptr;
    pthread_mutex_init(&mutex, nullptr);
    pthread_mutex_lock(&mutex);
}

wait_task::wait_task(void *(*_task)(void *), void *_arg)
{
    task = _task;
    arg = _arg;
}

thread_pool::thread_pool()
{
    sem_init(&idle_sem, 0, MAX_THREAD);
    sem_init(&wait_sem, 0, 0);

    for(int i=0;i<MAX_THREAD;i++)
    {
        init_arg[i].order = i;
        init_arg[i].pool = this;
        pthread_create(&init_arg[i].tid, nullptr, thread_loop, &init_arg[i]);
        idle_list.push_back(&thread_list[i]);
    }
    pthread_create(&sche_tid, nullptr, sche_loop, (void*)this);
}

thread_pool::~thread_pool()
{
    for(int i=0;i<MAX_THREAD;i++)
    {
        thread_list[i].task = thread_exit;
        pthread_mutex_unlock(&thread_list[i].mutex);
    }

    for(int i=0;i<MAX_THREAD;i++)
    {
        pthread_join(init_arg[i].tid, nullptr);
    }
}

int thread_pool::run(void* (*task)(void *), void *arg)
{
    struct wait_task temp(task, arg);
    wait_list.push_back(temp);
    sem_post(&wait_sem);

    return 0;
}

void *thread_loop(void * arg)
{
    struct thread_init* thi = (struct thread_init*)arg;

    while(1)
    {
        pthread_mutex_lock(&thi->pool->thread_list[thi->order].mutex);
        if(thi->pool->thread_list[thi->order].task == nullptr)
        {

        }
        else
        {
            thi->pool->thread_list[thi->order].task(thi->pool->thread_list[thi->order].arg);
            thi->pool->thread_list[thi->order].task = nullptr;
            thi->pool->thread_list[thi->order].arg = nullptr;
            thi->pool->idle_list.push_back(&thi->pool->thread_list[thi->order]);
            sem_post(&thi->pool->idle_sem);
        }
    }
}

void *sche_loop(void * arg)
{
    thread_pool* th_p = (thread_pool*)arg;
    while(1)
    {
        sem_wait(&th_p->wait_sem);
        sem_wait(&th_p->idle_sem);

        struct thread_info* ptr = th_p->idle_list.front();
        struct wait_task temp = th_p->wait_list.front();
        ptr->arg = temp.arg;
        ptr->task = temp.task;
        pthread_mutex_unlock(&ptr->mutex);
        th_p->idle_list.pop_front();
        th_p->wait_list.pop_front();
    }
}

void *thread_exit(void *)
{
    pthread_exit(0);
    return nullptr;
}

