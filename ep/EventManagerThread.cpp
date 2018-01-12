//
// Created by borelset on 1/12/18.
//

#include "EventManagerThread.h"

using namespace ep;

EventManagerThread::EventManagerThread():
        mThread(std::bind(&EventManagerThread::threadFunction, this)),
        mEventManager(nullptr),
        mCondition(mMutexLock)
{

}

EventManagerThread::~EventManagerThread() {

}

void EventManagerThread::start() {
    mThread.run();
    {
        MutexLockGuard localGuard(mMutexLock);
        mCondition.wait();
    }
}

EventManager *EventManagerThread::getEventManagerHandler() {
    return mEventManager;
}


void EventManagerThread::threadFunction() {
    EventManager eventManager;
    {
        MutexLockGuard localGuard(mMutexLock);
        mEventManager = &eventManager;
        mCondition.notify();
    }
    eventManager.loop();

}
