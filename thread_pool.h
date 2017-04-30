#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <pthread.h>
#define MAX_THREAD 70

class thread_pool;

struct thread_info
{
    int tag = 0;
    void* (*task)(void* arg) = nullptr;
    void* arg = nullptr;
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
    thread_info thread_list[MAX_THREAD];
    struct thread_init init_arg[MAX_THREAD];
    pthread_mutex_t mutex;
    int count = 0;
public:
    thread_pool();
    ~thread_pool();
    int run(void* (*)(void*), void* arg);
    friend void* thread_loop(void*);
};

void* thread_loop(void*);
void *thread_exit(void*);

#endif // THREAD_POOL_H
