#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <pthread.h>
#include <semaphore.h>
#include <list>
#define MAX_THREAD 10

class thread_pool;

struct thread_info
{
    pthread_mutex_t mutex;
    void* (*task)(void* arg);
    void* arg;
    thread_info();
};

struct wait_task
{
    void* (*task)(void* arg);
    void* arg;
    wait_task(void* (*)(void* arg), void*);
};

struct thread_init
{
    pthread_t tid;
    int order = -1;
    thread_pool* pool;
};

class thread_pool
{
private:
    struct thread_info thread_list[MAX_THREAD];
    struct thread_init init_arg[MAX_THREAD];
    std::list<struct thread_info*> idle_list;
    std::list<struct wait_task> wait_list;
    pthread_t sche_tid;
    sem_t idle_sem;
    sem_t wait_sem;
public:
    thread_pool();
    ~thread_pool();
    int run(void* (*)(void*), void* arg);
    friend void* thread_loop(void*);
    friend void* sche_loop(void*);
};

void* thread_loop(void*);
void* sche_loop(void*);
void* thread_exit(void*);

#endif // THREAD_POOL_H
