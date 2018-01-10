//
// Created by borelset on 1/10/18.
//

#ifndef NETMODEL_TIMERQUEUE_H
#define NETMODEL_TIMERQUEUE_H

#include <ctime>
#include <set>
#include <vector>
#include "Timer.h"
#include "Channel.h"

class TimerQueue{
public:
    typedef std::function<void()> TimerCallback;
    typedef std::pair<time_t, std::unique_ptr<Timer>> Entry;
    typedef std::set<Entry> TimerList;
    TimerQueue(EventManager* eventManager);
    std::vector<Entry> getExpired(time_t now);
    void handleRead();
    bool addTimer(TimerCallback& timerCallback, time_t time, int interval);
    void resetTimerFd(time_t time);
private:
    Channel mTimerQueueChannel;
    EventManager* mEventManager;
    const int mTimerFd;
    TimerList mTimerList;
};

#endif //NETMODEL_TIMERQUEUE_H
