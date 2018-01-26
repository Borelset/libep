//
// Created by borelset on 1/10/18.
//

#ifndef LIBEP_EP_TIMERQUEUE_H
#define LIBEP_EP_TIMERQUEUE_H

#include <ctime>
#include <set>
#include <vector>
#include <memory>
#include "Timer.h"
#include "Channel.h"
#include "../Utils/FD.h"

namespace ep{
    class TimerQueue : Utils::noncopyable{
    public:
        typedef std::function<void()> TimerCallback;
        typedef std::weak_ptr<Timer> TimerPtr;
        typedef std::pair<time_t, std::shared_ptr<Timer>> Entry;
        typedef std::set<Entry> TimerList;
        explicit TimerQueue(EventManager* eventManager);
        ~TimerQueue();
        TimerPtr addTimer(const TimerCallback& timerCallback, time_t time, int interval);
        void removeTimer(TimerPtr&);
    private:
        Utils::FD mTimerFd;
        EventManager* mEventManager;
        TimerList mTimerList;
        Channel mTimerQueueChannel;

        TimerPtr addTimerInQueue(std::shared_ptr<Timer> timer);
        void handleRead();
        std::vector<Entry> getExpired(time_t now);
        void resetTimerFd(time_t time);
        void removeTimerInQueue(TimerPtr&);
    };
}

#endif //LIBEP_EP_TIMERQUEUE_H
