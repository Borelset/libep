//
// Created by borelset on 1/3/18.
//
#include "Mutex.h"
#include "CurrentThread.h"

using namespace ep;

void MutexLock::lock(){
    pthread_mutex_lock(&mMutex);
    mHolder = CurrentThread::gettid();
}

void MutexLock::unlock() {
    mHolder = 0;
    pthread_mutex_unlock(&mMutex);
}

pthread_mutex_t *MutexLock::getPthreadMutex() {
    return &mMutex;
}

MutexLock::MutexLock():mHolder(0){
    pthread_mutexattr_settype(&mMutexType, PTHREAD_MUTEX_NORMAL);
    pthread_mutex_init(&mMutex, &mMutexType);
}

MutexLock::~MutexLock(){
    if(mHolder!=0)
        std::cout << "Mutex held" << std::endl;
    pthread_mutex_destroy(&mMutex);
}

bool MutexLock::isLockByThisThread() {
    return mHolder == CurrentThread::gettid();
}

MutexLockGuard::MutexLockGuard(MutexLock &mutex):mMutexLock(mutex){
    mMutexLock.lock();
}

MutexLockGuard::~MutexLockGuard(){
    mMutexLock.unlock();
}

Condition::Condition(MutexLock &mutex):mMutexLock(mutex){
    pthread_cond_init(&mCond, nullptr);
}

Condition::~Condition(){
    pthread_cond_destroy(&mCond);
}

void Condition::wait(){
    pthread_cond_wait(&mCond, mMutexLock.getPthreadMutex());
}

void Condition::notify(){
    pthread_cond_signal(&mCond);
}

void Condition::notifyAll(){
    pthread_cond_broadcast(&mCond);
}