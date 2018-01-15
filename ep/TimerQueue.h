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

namespace ep{
    class TimerQueue{
    public:
        typedef std::function<void()> TimerCallback;
        typedef std::pair<time_t, Timer*> Entry;
        typedef std::set<Entry> TimerList;
        TimerQueue(EventManager* eventManager);
        std::vector<Entry> getExpired(time_t now);
        void handleRead();
        bool addTimerInQueue(Timer *timer);
        bool addTimer(const TimerCallback& timerCallback, time_t time, int interval);
        void resetTimerFd(time_t time);
    private:
        const int mTimerFd;
        EventManager* mEventManager;
        TimerList mTimerList;
        Channel mTimerQueueChannel;

    };
}

#endif //NETMODEL_TIMERQUEUE_H
