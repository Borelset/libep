//
// Created by borelset on 1/10/18.
//

#include "EventManager.h"
#include "EpollHandler.h"
#include "TimerQueue.h"

EventManager::EventManager(int ThreadId):
        mEpollHandlerPtr(new EpollHandler()),
        mQuit(true),
        mTimerQueue(this),
        mThreadId(ThreadId)
{

}

EventManager::~EventManager() {

}

void EventManager::loop() {
    ChannelList wakedChannel;
    while(mQuit){
        wakedChannel.clear();
        mEpollHandlerPtr->epoll(100, &wakedChannel);
        for(ChannelList::iterator it = wakedChannel.begin();
                it != wakedChannel.end();
                it++){
            (*it)->process();
        }
    }
}

void EventManager::updateChannel(Channel * channel) {
    mEpollHandlerPtr->updateChannel(channel);
}

void EventManager::setQuit() {
    mQuit = false;
}

void EventManager::runAt(std::function<void()> Callback, time_t when, int interval) {
    time_t time_ = getTime();
    if(when - time_ > 0)
        mTimerQueue.addTimer(Callback, when-getTime(), interval);
    else
        std::cout << "EventManger::runAt() invalid param"
                  << std::endl
                  << "when == " << when << " smaller than getTime() == " << time_ << std::endl;
}

void EventManager::runAfter(std::function<void()> Callback, time_t when, int interval) {
    if(when <=0){
        std::cout << "EventManger::runAfter() invalid param"
                  << std::endl
                  << "when == " << when << " smaller than 0" << std::endl;
    }
    mTimerQueue.addTimer(Callback, when, interval);
}

bool EventManager::isNativeThread(pid_t threadId) {
    return threadId == mThreadId;
}

void EventManager::testExist() {
    std::cout << "i`m here: " << mThreadId << std::endl;
}
