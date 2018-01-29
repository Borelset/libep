//
// Created by borelset on 1/10/18.
//

#include "EventManager.h"
#include "EpollHandler.h"
#include "TimerQueue.h"
#include "../Utils/CurrentThread.h"
#include "../Utils/Logger/Logger.h"
#include "../Utils/Logger/LoggerManager.h"
#include <sys/eventfd.h>

using namespace ep;

EventManager::EventManager():
        mEpollHandlerPtr(new EpollHandler()),
        mQuit(true),
        mTimerQueue(this),
        mThreadId(Utils::CurrentThread::gettid()),
        mEventFd(eventfd(0, EFD_NONBLOCK|EFD_CLOEXEC)),
        mWakeupChannel(this, mEventFd.getFd()),
        mEventfdCallbackProcessing(false)
{
    mWakeupChannel.setReadCallback(std::bind(&EventManager::handleRead, this));
    mWakeupChannel.enableRead();
}

EventManager::~EventManager() {

}

void EventManager::loop() {
    ChannelList wakedChannel;
    while(mQuit){
        wakedChannel.clear();
        //-1 for infinity wait
        mEpollHandlerPtr->epoll(-1, &wakedChannel);
        for(auto it = wakedChannel.begin();
                it != wakedChannel.end();
                it++){
            (*it)->process();
        }
        eventfdCallbackProcess();
    }
}

void EventManager::updateChannel(Channel * channel) {
    mEpollHandlerPtr->updateChannel(channel);
}

void EventManager::setQuit() {
    mQuit = false;
    if(!isLocalThread())
        wakeup();
}

std::weak_ptr<Timer> EventManager::runAt(std::function<void()> Callback,
                         time_t when,
                         int interval) {
    time_t time_ = Utils::getTime();
    std::weak_ptr<Timer> result;
    if(when - time_ > 0)
        result = mTimerQueue.addTimer(Callback, when-Utils::getTime(), interval);
    else
        Log::LogWarning << "ep::EventManger::runAt() invalid param"
                        << Log::endl
                        << "when == " << when << " smaller than getTime() == " << time_ << Log::endl;
    return result;
}

std::weak_ptr<Timer> EventManager::runAfter(std::function<void()> Callback,
                            time_t when,
                            int interval) {
    std::weak_ptr<Timer> result;
    if(when <=0){
        Log::LogWarning << "ep::EventManger::runAfter==>"
                        <<"invalid param. "
                        << "when == " << when << " smaller than 0" << Log::endl;
        return result;
    }
    return mTimerQueue.addTimer(Callback, when, interval);
}

bool EventManager::isLocalThread() {
    bool result =
            mThreadId == Utils::CurrentThread::gettid();
    if(!result){
        Log::LogInfo << "ep::EventManager::isLocalThread==>"
                     << "EventManager created in:" << mThreadId << " isLocalThread called in:" << Utils::CurrentThread::gettid() << Log::endl;
    }
    return result;
}

void EventManager::runInLoop(const EventManager::Callback &callback) {
    if(isLocalThread()){
        callback();
    }else{
        Log::LogInfo << "ep::EventManager::runInLoop==>"
                     << "called by foreign thread:" << Utils::CurrentThread::gettid() << " and turn to queueInLoop" << Log::endl;
        queueInLoop(callback);
    }
}

void EventManager::queueInLoop(const EventManager::Callback &callback) {
    Log::LogInfo << "ep::EventManager::queueInLoop==>"
                 << "mutex" << Log::endl;
    {
        Utils::MutexLockGuard localGuard(mMutexLock);
        mCallbackQueue.push_back(callback);
    }

    Log::LogInfo << "ep::EventManager::queueInLoop==>"
                 << "judging wakeup" << Log::endl;
    if(mEventfdCallbackProcessing || !isLocalThread())
        wakeup();
}

void EventManager::handleRead() {
    uint64_t many;
    read(mEventFd.getFd(), &many, sizeof many);
    Log::LogInfo << "ep::EventManager::handleRead==>"
                 << "Eventfd found " << many << " item(s)" << Log::endl;
}

void EventManager::wakeup() {
    uint64_t one = 1;
    write(mEventFd.getFd(), &one, sizeof one);
    Log::LogInfo << "ep::EventManager::wakeup==>"
                 << "send a signal to wakeup" << Log::endl;
}

void EventManager::eventfdCallbackProcess() {
    Log::LogInfo << "EventManager::eventfdCallbackProcess==>"
                 << Log::endl;
    mEventfdCallbackProcessing = true;
    CallbackList callbackList;
    {
        Utils::MutexLockGuard localGuard(mMutexLock);
        mCallbackQueue.swap(callbackList);
    }

    for(int i=0; i<callbackList.size(); i++){
        callbackList[i]();
    }
    mEventfdCallbackProcessing = false;
}

void EventManager::removeChannel(Channel * channel) {
    Log::LogInfo << "ep::EventManager::removeChannel==>"
                 << "Channel " << channel->getFd() << " removed" << Log::endl;
    mEpollHandlerPtr->removeChannel(channel);
}

void EventManager::stopTimer(std::weak_ptr<Timer> timer) {
    Log::LogInfo << "ep::EventManager::stopTimer==>"
                 << "stop timer" << Log::endl;
    mTimerQueue.removeTimer(timer);
}

