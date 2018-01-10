//
// Created by borelset on 1/10/18.
//

#include <functional>
#include "Timer.h"

Timer::Timer(std::function<void()> &timerCallback, time_t time, int interval):
        mTimerCallback(timerCallback),
        mStartTime(time),
        mInterval(interval)
{

}

void Timer::run() {
    mTimerCallback();
}
