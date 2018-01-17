//
// Created by borelset on 1/17/18.
//

#ifndef LIBEP_EVENTMANAGERTHREADPOOL_H
#define LIBEP_EVENTMANAGERTHREADPOOL_H

#include <memory>
#include "../Utils/noncopyable.h"
#include "EventManager.h"
#include "EventManagerThread.h"

namespace ep{
    class EventManagerThreadPool : Utils::noncopyable{
    public:
        explicit EventManagerThreadPool(EventManager* eventManager);
        ~EventManagerThreadPool();
        void setThreadNum(int num);
        void start();
        EventManager* getNextManager();
    private:
        EventManager* mBaseEventManager;
        bool mStart;
        int mThreadCount;
        int mNextIndex;
        std::vector<std::shared_ptr<EventManagerThread>> mThreads;
        std::vector<EventManager*> mManagerHandles;
    };
}

#endif //LIBEP_EVENTMANAGERTHREADPOOL_H
