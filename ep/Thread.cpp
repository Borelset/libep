//
// Created by borelset on 1/12/18.
//

#include <functional>
#include "Thread.h"
#include "CurrentThread.h"

using namespace ep;

Thread::Thread(const std::function<void()> &callback):
        mCallback(callback),
        mIsRunning(false)
{

}

Thread::~Thread() {

}

void Thread::run() {
    std::cout << "Thread::run==>"
              << "pthread_create" << std::endl;
    pthread_create(&mPid, nullptr, CurrentThread::callbackProxy, this);
}

int Thread::join() {
    return pthread_join(mPid, nullptr);
}

bool Thread::isRunning() {
    return mIsRunning;
}

Thread::Callback Thread::getCallback() {
    return mCallback;
}

void Thread::setTid(pid_t tid) {
    mTid = tid;
}
