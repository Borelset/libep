//
// Created by borelset on 1/10/18.
//

#ifndef NETMODEL_TIMER_H
#define NETMODEL_TIMER_H

#include <functional>

namespace ep{
    class Timer{
    public:
        typedef std::function<void()> TimerCallback;
        Timer(const TimerCallback& timerCallback, time_t time, int interval);
        void run();
        bool isRepeat();
        Timer* getRepeat();
        time_t getStartTime();
    private:
        TimerCallback mTimerCallback;
        int mInterval;
        time_t mStartTime;
    };
}


#endif //NETMODEL_TIMER_H
