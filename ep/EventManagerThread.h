//
// Created by borelset on 1/12/18.
//

#ifndef NETMODEL_EVENTLOOPTHREAD_H
#define NETMODEL_EVENTLOOPTHREAD_H

#include "Thread.h"
#include "EventManager.h"

namespace ep{
    class EventManagerThread{
    public:
        EventManagerThread();
        ~EventManagerThread();
        void start();
        EventManager* getEventManagerHandler();
    private:
        ep::Thread mThread;
        EventManager* mEventManager;
        MutexLock mMutexLock;
        Condition mCondition;

        void threadFunction();
    };
}


#endif //NETMODEL_EVENTLOOPTHREAD_H
