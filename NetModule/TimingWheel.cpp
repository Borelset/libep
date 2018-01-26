//
// Created by borelset on 1/18/18.
//

#include "TCPConnectionHolder.h"
#include "TimingWheel.h"
#include "TCPConnection.h"
#include "../Utils/Logger/LoggerManager.h"

NetModule::TimingWheel::TimingWheel(int time) :
        mWheel(time),
        mCircle(time),
        mWheelPtr(0)
{

}

void NetModule::TimingWheel::refresh(std::shared_ptr<TCPConnectionHolder> tch) {
    Log::LogInfo << "NetModule::TimingWheel::refresh==>"
                 << tch->getTCPConnectionPtr().lock()->getName().c_str() << " refreshed" << Log::endl;
    Utils::MutexLockGuard localGuard(mMutexLock);
    mWheel[mWheelPtr].push_back(tch);
}

void NetModule::TimingWheel::goAhead() {
    Log::LogInfo << "NetModule::TimingWheel::goAhead==>"
                 << "mWheelPtr:" << mWheelPtr << Log::endl;
    Utils::MutexLockGuard localGuard(mMutexLock);
    mWheelPtr = (mWheelPtr+1)%mCircle;
    mWheel[mWheelPtr].clear();
}

void NetModule::TimingWheel::setTimingCircle(int n) {
    if(n < mCircle){
        Log::LogWarning << "NetModule::TimingWheel::setTimingCircle==>"
                        << "new size is smaller than old one, some connection will be released" << Log::endl;
    }
    mWheel.resize(n);
    mCircle = n;
}
