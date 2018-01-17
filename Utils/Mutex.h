//
// Created by borelset on 1/3/18.
//

#ifndef LIBEP_UTILS_UTILS_MUTEX_H
#define LIBEP_UTILS_UTILS_MUTEX_H

#include <iostream>
#include "pthread.h"
#include "../ep/epollUtils.h"
#include "noncopyable.h"

namespace Utils{
    class MutexLockGuard;
    class Condition;

    class MutexLock : noncopyable{
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
        int getHolder();
    };

    class MutexLockGuard : noncopyable{
    private:
        MutexLock& mMutexLock;
    public:
        explicit MutexLockGuard(MutexLock& mutex);
        ~MutexLockGuard();
    };

    class Condition : noncopyable{
    private:
        MutexLock& mMutexLock;
        pthread_cond_t mCond;
    public:
        explicit Condition(MutexLock& mutex);
        ~Condition();
        void wait();
        void notify();
        void notifyAll();
    };
}



#endif //LIBEP_UTILS__MUTEX_H
