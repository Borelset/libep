//
// Created by borelset on 1/18/18.
//

#ifndef LIBEP_TIMINGWHEEL_H
#define LIBEP_TIMINGWHEEL_H

#include "../Utils/noncopyable.h"
#include "TCPConnectionHolder.h"
#include "../Utils/Mutex.h"
#include <list>
#include <vector>

namespace NetModule{
    class TimingWheel : Utils::noncopyable{
    public:
        explicit TimingWheel(int time);
        void refresh(std::shared_ptr<TCPConnectionHolder>);
        void goAhead();
    private:
        std::vector<std::list<std::shared_ptr<TCPConnectionHolder>>> mWheel;
        int mCircle;
        int mWheelPtr;
        Utils::MutexLock mMutexLock;
    };
}

#endif //LIBEP_TIMINGWHEEL_H
