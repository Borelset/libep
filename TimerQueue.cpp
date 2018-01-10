//
// Created by borelset on 1/10/18.
//

#include "EventManager.h"
#include "TimerQueue.h"
#include <sys/timerfd.h>

TimerQueue::TimerQueue(EventManager *eventManager):
        mEventManager(eventManager),
        mTimerFd(timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC)),
        mTimerQueueChannel(eventManager, mTimerFd)
{
    mTimerQueueChannel.setReadCallback(std::bind(handleRead, *this));
    mTimerQueueChannel.enableRead();
}

std::vector<TimerQueue::Entry> TimerQueue::getExpired(time_t now) {
    std::vector<Entry> result;
    Entry boundary = std::make_pair(now, (Timer*)UINTPTR_MAX);
    TimerList::iterator it = mTimerList.lower_bound(boundary);
    std::copy(mTimerList.begin(), it, std::back_inserter(result));
    mTimerList.erase(mTimerList.begin(), it);
    return result;
}

void TimerQueue::handleRead() {
    std::vector<Entry> timeout = getExpired(time_t());
    for(auto it = timeout.begin(); it != timeout.end(); it++){
        it->second->run();
    }
}

bool TimerQueue::addTimer(TimerQueue::TimerCallback &timerCallback, time_t time, int interval) {
    std::unique_ptr<Timer> newTimer(new Timer(timerCallback, time, interval));
    Entry newEntry = std::make_pair(time, newTimer);

    if(mTimerList.begin()->first > time){
        resetTimerFd(time);
        mTimerList.insert(newEntry);
        return true;
    }else{
        mTimerList.insert(newEntry);
        return false;
    }
}

void TimerQueue::resetTimerFd(time_t time) {
    timerfd_settime(mTimerFd, )
}
