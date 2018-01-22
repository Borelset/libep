//
// Created by borelset on 1/10/18.
//

#include "EventManager.h"
#include "TimerQueue.h"
#include "../Utils/Utils.h"
#include "../Utils/Logger/LoggerManager.h"
#include <sys/timerfd.h>
#include <string.h>
#include <unistd.h>

using namespace ep;

int createTimerFd(){
    int timerFd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    Log::LogInfo << "ep::TimerQueue::createTimerFd==>"
                 << "created timerFd: " << timerFd << Log::endl;
    return timerFd;
}

TimerQueue::TimerQueue(EventManager *eventManager):
        mEventManager(eventManager),
        mTimerFd(createTimerFd()),
        mTimerQueueChannel(eventManager, mTimerFd.getFd())
{
    mTimerQueueChannel.setReadCallback(std::bind(&TimerQueue::handleRead, this));
    mTimerQueueChannel.enableRead();
}

std::vector<TimerQueue::Entry> TimerQueue::getExpired(time_t now) {
    std::vector<Entry> result;
    Entry boundary = std::make_pair(now, (Timer*)UINTPTR_MAX);
    TimerList::iterator it = mTimerList.lower_bound(boundary);
    std::copy(mTimerList.begin(), it, std::back_inserter(result));
    mTimerList.erase(mTimerList.begin(), it);
    Log::LogInfo << "ep::TimerQueue::getExpired==>"
                 << "Found " << result.size() << " items, and left " << mTimerList.size() << Log::endl;
    if(!mTimerList.empty())
        resetTimerFd(mTimerList.begin()->first - Utils::getTime());
    return result;
}

void TimerQueue::handleRead() {
    uint64_t many;
    read(mTimerFd.getFd(), &many, sizeof many);
    Log::LogInfo << "ep::TimerQueue::handleRead==>"
                 << "There is " << many << " timer(s) time out" << Log::endl;
    std::vector<Entry> timeout = getExpired(Utils::getTime());
    for(auto it = timeout.begin(); it != timeout.end(); it++){
        it->second->run();
        if(it->second->isRepeat()){
            Timer* repeatTimer = it->second->getRepeat();
            addTimerInQueue(repeatTimer);
        }else{
            delete it->second;
        }
    }
}


bool TimerQueue::addTimerInQueue(Timer *timer) {
    Entry newEntry = std::make_pair(timer->getStartTime(), timer);

    if(mTimerList.begin()->first > timer->getStartTime() || mTimerList.empty()){
        resetTimerFd(timer->getStartTime() - Utils::getTime());
        mTimerList.insert(newEntry);
        return true;
    }else{
        mTimerList.insert(newEntry);
        return false;
    }
}

bool TimerQueue::addTimer(const TimerQueue::TimerCallback &timerCallback,
                          time_t time,
                          int interval) {
    time_t now = Utils::getTime();
    Timer* newTimer = new Timer(timerCallback, time+now, interval);
    TimerCallback added = std::bind(&TimerQueue::addTimerInQueue, this, newTimer);
    mEventManager->runInLoop(added);
    return true;
}

void TimerQueue::resetTimerFd(time_t time) {
    struct itimerspec now;
    bzero(&now, sizeof(itimerspec));
    now.it_value.tv_sec = time;
    timerfd_settime(mTimerFd.getFd(), 0, &now, nullptr);
    Log::LogInfo << "ep::TimerQueue::resetTimerFd==>"
                 << "set alert after " << time << "second(s)" << Log::endl;
}

TimerQueue::~TimerQueue() {
}
