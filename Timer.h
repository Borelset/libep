//
// Created by borelset on 1/10/18.
//

#ifndef NETMODEL_TIMER_H
#define NETMODEL_TIMER_H

#include <functional>

class Timer{
public:
    typedef std::function<void()> TimerCallback;
    Timer(TimerCallback& timerCallback, time_t time, int interval);
    void run();
private:
    TimerCallback mTimerCallback;
    int mInterval;
    time_t mStartTime;
};

#endif //NETMODEL_TIMER_H
