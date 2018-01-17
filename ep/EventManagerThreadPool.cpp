//
// Created by borelset on 1/17/18.
//

#include "EventManager.h"
#include "EventManagerThreadPool.h"

ep::EventManagerThreadPool::EventManagerThreadPool(EventManager *eventManager) :
        mBaseEventManager(eventManager),
        mStart(false),
        mThreadCount(0),
        mNextIndex(0)
{

}

ep::EventManagerThreadPool::~EventManagerThreadPool() {

}

void ep::EventManagerThreadPool::setThreadNum(int num) {
    mThreadCount = num;
    for(int i=0; i<num; i++){
        std::shared_ptr<EventManagerThread> newThread(new EventManagerThread());
        mThreads.push_back(newThread);
    }
    std::cout << "ep::EventManagerThreadPool::setThreadNum==>"
              << mThreadCount << " Threads established.." << std::endl;
}

void ep::EventManagerThreadPool::start() {
    for(auto i = mThreads.begin(); i!=mThreads.end(); i++){
        (*i)->start();
        mManagerHandles.push_back(((*i)->getEventManagerHandler()));
    }
    std::cout << "ep::EventManagerThreadPool::start==>"
              << mThreadCount << " Threads started.." << std::endl;
}

ep::EventManager *ep::EventManagerThreadPool::getNextManager() {
    if(mThreadCount == 0){
        return mBaseEventManager;
    }else{
        mNextIndex = (mNextIndex+1) % mThreadCount;
        std::cout << "ep::EventManagerThreadPool::getNextManager==>"
                  << "Thread " << mNextIndex << " provided" << std::endl;
        return mManagerHandles[mNextIndex];
    }
}
