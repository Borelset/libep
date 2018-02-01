//
// Created by borelset on 1/12/18.
//

#include "EventManagerThread.h"
#include "../Utils/CurrentThread.h"
#include "../Utils/Logger/LoggerManager.h"

using namespace ep;

EventManagerThread::EventManagerThread():
        mThread(std::bind(&EventManagerThread::threadFunction, this)),
        mEventManager(nullptr),
        mCondition(mMutexLock),
        mTid(mThread.getTid())
{

}

EventManagerThread::~EventManagerThread() {

}

void EventManagerThread::start() {
    mThread.run();
    {
        Utils::MutexLockGuard localGuard(mMutexLock);
        mCondition.wait();
    }
}

EventManager *EventManagerThread::getEventManagerHandler() {
    return mEventManager;
}


void EventManagerThread::threadFunction() {
    Log::LogInfo << "ep::EventManagerThread::threadFunction==>"
                 << "Thread created:" << Utils::CurrentThread::gettid() << Log::endl;
    EventManager eventManager;
    {
        Utils::MutexLockGuard localGuard(mMutexLock);
        mEventManager = &eventManager;
        mCondition.notify();
    }
    eventManager.loop();
}

void EventManagerThread::stop() {
    mEventManager->setQuit();
    mThread.join();
}
