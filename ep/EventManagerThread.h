//
// Created by borelset on 1/12/18.
//

#ifndef LIBEP_EP_EVENTLOOPTHREAD_H
#define LIBEP_EP_EVENTLOOPTHREAD_H

#include "../Utils/Thread.h"
#include "EventManager.h"

namespace ep{
    class EventManagerThread : Utils::noncopyable{
    public:
        EventManagerThread();
        ~EventManagerThread();
        void start();
        EventManager* getEventManagerHandler();
    private:
        Utils::Thread mThread;
        EventManager* mEventManager;
        Utils::MutexLock mMutexLock;
        Utils::Condition mCondition;
        pid_t mTid;

        void threadFunction();
    };
}


#endif //LIBEP_EP_EVENTLOOPTHREAD_H
