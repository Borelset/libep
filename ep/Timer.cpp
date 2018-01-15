//
// Created by borelset on 1/10/18.
//

#include <functional>
#include "Timer.h"
#include "epollUtils.h"

using namespace ep;

Timer::Timer(const TimerCallback &timerCallback, time_t time, int interval):
        mTimerCallback(timerCallback),
        mStartTime(time),
        mInterval(interval)
{

}

void Timer::run() {
    mTimerCallback();
}

bool Timer::isRepeat() {
    return mInterval != 0;
}

Timer *Timer::getRepeat() {
    mStartTime += mInterval;
    return this;
}

time_t Timer::getStartTime() {
    return mStartTime;
}
