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

    Entry boundary = std::make_pair(
            now, std::shared_ptr<Timer>((Timer*)UINTPTR_MAX, [](Timer*){})     //need special dtor(do nothing)
    );

    TimerList::iterator it = mTimerList.lower_bound(boundary);
    std::copy(mTimerList.begin(), it, std::back_inserter(result));
    mTimerList.erase(mTimerList.begin(), it);                        // shared_ptr erase from mTimerList
    Log::LogInfo << "ep::TimerQueue::getExpired==>"
                 << "Found " << result.size() << " item(s) timeout, and left " << mTimerList.size() << Log::endl;
    if(!mTimerList.empty())
        resetTimerFd(mTimerList.begin()->first - Utils::getTime());  // set new timer

    return result;                                                   // caution! return including shared_ptr
                                                                     // ensure usage WILL NOT lead to leak
}

void TimerQueue::handleRead() {
    uint64_t many;
    read(mTimerFd.getFd(), &many, sizeof many);
    Log::LogInfo << "ep::TimerQueue::handleRead==>"
                 << "There is " << many << " timer(s) time out" << Log::endl;
    std::vector<Entry> timeout = getExpired(Utils::getTime());       // get shared_ptr
    for(auto it = timeout.begin(); it != timeout.end(); it++){
        it->second->run();
        if(it->second->isRepeat()){
            it->second->getRepeat();
            addTimerInQueue(it->second);                             // add back
        }else{
            Log::LogInfo << "ep::TimerQueue::handleRead==>"
                         << "auto release" << Log::endl;
            //shared_ptr auto release;                               // ignore to release
        }
    }                                                                // return of getExpired release
}


std::weak_ptr<Timer> TimerQueue::addTimerInQueue(std::shared_ptr<Timer> timer) {
    Entry newEntry = std::make_pair(timer->getStartTime(), timer);

    if(mTimerList.begin()->first > timer->getStartTime() || mTimerList.empty()){
        resetTimerFd(timer->getStartTime() - Utils::getTime());
        mTimerList.insert(newEntry);  // shared saved in mTimerList
    }else{
        mTimerList.insert(newEntry);
    }
    return timer; // convert to weak
}

std::weak_ptr<Timer> TimerQueue::addTimer(const TimerQueue::TimerCallback &timerCallback,
                          time_t time,
                          int interval) {
    Log::LogInfo << "ep::TimerQueue::addTimer==>"
                 << "add a timer" << Log::endl;
    time_t now = Utils::getTime();
    std::shared_ptr<Timer> newTimer(new Timer(timerCallback, time+now, interval));
    mEventManager->runInLoop(
            std::bind(&TimerQueue::addTimerInQueue, this, newTimer)
    );
    return newTimer; // convert to weak
}

void TimerQueue::resetTimerFd(time_t time) {
    struct itimerspec now;
    bzero(&now, sizeof(itimerspec));
    now.it_value.tv_sec = time;
    timerfd_settime(mTimerFd.getFd(), 0, &now, nullptr);
    Log::LogInfo << "ep::TimerQueue::resetTimerFd==>"
                 << "set alert after " << time << " second(s)" << Log::endl;
}

TimerQueue::~TimerQueue() {
}

void TimerQueue::removeTimer(std::weak_ptr<Timer>& timer) {
    Log::LogInfo << "ep::TimerQueue::removeTimer==>"
                 << "removeTimer" << Log::endl;
    mEventManager->runInLoop(
            std::bind(&TimerQueue::removeTimerInQueue, this, timer)
    );
}

void TimerQueue::removeTimerInQueue(TimerQueue::TimerPtr & timer) {
    Log::LogInfo << "ep::TimerQueue::removeTimerInQueue==>"
                 << "removeTimer" << Log::endl;
    auto sPtr = timer.lock();
    if(sPtr == nullptr) return;
    auto searchEntry = std::make_pair<time_t, std::shared_ptr<Timer>>(
            sPtr->getStartTime(), timer.lock()
    );

    auto tempTimer = mTimerList.find(searchEntry);
    if(tempTimer!= mTimerList.end()){
        if(tempTimer == mTimerList.begin()){
            auto iter = mTimerList.begin();
            iter++;
            resetTimerFd(iter->first);
        }
        mTimerList.erase(tempTimer);
        return;
    }
    return;
}
