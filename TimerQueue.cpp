//
// Created by borelset on 1/10/18.
//

#include "EventManager.h"
#include "TimerQueue.h"
#include <sys/timerfd.h>
#include <string.h>
#include <zconf.h>

int createTimerFd(){
    int timerFd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    std::cout << "TimerFd: " << timerFd << std::endl;
    return timerFd;
}

TimerQueue::TimerQueue(EventManager *eventManager):
        mEventManager(eventManager),
        mTimerFd(createTimerFd()),
        mTimerQueueChannel(Channel(eventManager, mTimerFd))
{
    mTimerQueueChannel.setReadCallback(std::bind(&TimerQueue::handleRead, this));
    mTimerQueueChannel.enableRead();
}

std::vector<TimerQueue::Entry> TimerQueue::getExpired(time_t now) {
    std::vector<Entry> result;
    Entry boundary = std::make_pair(now, (Timer*)UINTPTR_MAX);
    TimerList::iterator it = mTimerList.lower_bound(boundary);
    std::copy(mTimerList.begin(), it, std::back_inserter(result));
    std::cout << "now:" << getTime() << std::endl;
    std::cout << "mTimerList size:" << mTimerList.size() << " result size:" << result.size() << std::endl;
    mTimerList.erase(mTimerList.begin(), it);
    if(!mTimerList.empty())
        resetTimerFd(mTimerList.begin()->first - getTime());
    return result;
}

void TimerQueue::handleRead() {
    uint64_t many;
    read(mTimerFd, &many, sizeof many);
    std::cout << many << " item(s) time out" << std::endl;
    std::vector<Entry> timeout = getExpired(getTime());
    for(auto it = timeout.begin(); it != timeout.end(); it++){
        it->second->run();
        if(it->second->isRepeat()){
            Timer* repeatTimer = it->second->getRepeat();
            addTimer(repeatTimer);
        }else{
            delete it->second;
        }
    }
}


bool TimerQueue::addTimer(Timer *timer) {
    Entry newEntry = std::make_pair(timer->getStartTime(), timer);

    if(mTimerList.begin()->first > timer->getStartTime() || mTimerList.empty()){
        resetTimerFd(timer->getStartTime() - getTime());
        mTimerList.insert(newEntry);
        return true;
    }else{
        mTimerList.insert(newEntry);
        return false;
    }
}



bool TimerQueue::addTimer(const TimerQueue::TimerCallback &timerCallback, time_t time, int interval) {
    time_t now = getTime();
    Timer* newTimer = new Timer(timerCallback, time+now, interval);
    return addTimer(newTimer);
}

void TimerQueue::resetTimerFd(time_t time) {
    struct itimerspec now;
    bzero(&now, sizeof(itimerspec));
    now.it_value.tv_sec = time;
    timerfd_settime(mTimerFd, 0, &now, nullptr);
    std::cout << "timeset:" << time << std::endl;
}

void TimerQueue::testChannelCallback() {
    mTimerQueueChannel.setRevent(EpollReadEvent);
    mTimerQueueChannel.process();
}
