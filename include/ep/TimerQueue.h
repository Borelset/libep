//
// Created by borelset on 1/10/18.
//

#ifndef LIBEP_EP_TIMERQUEUE_H
#define LIBEP_EP_TIMERQUEUE_H

#include <ctime>
#include <set>
#include <vector>
#include "Timer.h"
#include "Channel.h"
#include "../Utils/FD.h"

namespace ep{
    class TimerQueue : Utils::noncopyable{
    public:
        typedef std::function<void()> TimerCallback;
        typedef std::pair<time_t, Timer*> Entry;
        typedef std::set<Entry> TimerList;
        explicit TimerQueue(EventManager* eventManager);
        ~TimerQueue();
        std::vector<Entry> getExpired(time_t now);
        void handleRead();
        bool addTimerInQueue(Timer *timer);
        bool addTimer(const TimerCallback& timerCallback, time_t time, int interval);
        void resetTimerFd(time_t time);
    private:
        Utils::FD mTimerFd;
        EventManager* mEventManager;
        TimerList mTimerList;
        Channel mTimerQueueChannel;

    };
}

#endif //LIBEP_EP_TIMERQUEUE_H
