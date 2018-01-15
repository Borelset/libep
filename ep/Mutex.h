//
// Created by borelset on 1/3/18.
//

#ifndef MULTITHREAD_MUTEX_H
#define MULTITHREAD_MUTEX_H

#include <iostream>
#include "pthread.h"

namespace ep{
    class MutexLockGuard;
    class Condition;

    class MutexLock{
    private:
        pthread_mutex_t mMutex;
        pid_t mHolder;
        pthread_mutexattr_t mMutexType;
        friend MutexLockGuard;
        friend Condition;

        void lock();
        void unlock();
        pthread_mutex_t* getPthreadMutex();
    public:
        MutexLock();
        ~MutexLock();
        bool isLockByThisThread();
    };

    class MutexLockGuard{
    private:
        MutexLock& mMutexLock;
    public:
        MutexLockGuard(MutexLock& mutex);
        ~MutexLockGuard();
    };

    class Condition{
    private:
        MutexLock& mMutexLock;
        pthread_cond_t mCond;
    public:
        Condition(MutexLock& mutex);
        ~Condition();
        void wait();
        void notify();
        void notifyAll();
    };
}



#endif //MULTITHREAD_MUTEX_H
