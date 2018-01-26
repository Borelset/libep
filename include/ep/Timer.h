//
// Created by borelset on 1/10/18.
//

#ifndef LIBEP_EP_TIMER_H
#define LIBEP_EP_TIMER_H

#include <functional>
#include <memory>
#include "epollUtils.h"
#include "../Utils/noncopyable.h"

namespace ep{
    class Timer : Utils::noncopyable
    {
    public:
        typedef std::function<void()> TimerCallback;
        Timer(const TimerCallback& timerCallback, time_t time, int interval);
        ~Timer();
        void run();
        bool isRepeat();
        void getRepeat();
        time_t getStartTime();
    private:
        TimerCallback mTimerCallback;
        int mInterval;
        time_t mStartTime;
    };
}


#endif //LIBEP_EP_TIMER_H
