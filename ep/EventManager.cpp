//
// Created by borelset on 1/10/18.
//

#include "EventManager.h"
#include "EpollHandler.h"
#include "TimerQueue.h"
#include "CurrentThread.h"
#include <sys/eventfd.h>

EventManager::EventManager():
        mEpollHandlerPtr(new EpollHandler()),
        mQuit(true),
        mTimerQueue(this),
        mThreadId(CurrentThread::gettid()),
        mEventFd(eventfd(0, EFD_NONBLOCK|EFD_CLOEXEC)),
        mWakeupChannel(this, mEventFd),
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
        mEpollHandlerPtr->epoll(100, &wakedChannel);
        for(ChannelList::iterator it = wakedChannel.begin();
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

void EventManager::runAt(std::function<void()> Callback,
                         time_t when,
                         int interval) {
    time_t time_ = getTime();
    if(when - time_ > 0)
        mTimerQueue.addTimer(Callback, when-getTime(), interval);
    else
        std::cout << "EventManger::runAt() invalid param"
                  << std::endl
                  << "when == " << when << " smaller than getTime() == " << time_ << std::endl;
}

void EventManager::runAfter(std::function<void()> Callback,
                            time_t when,
                            int interval) {
    if(when <=0){
        std::cout << "EventManger::runAfter() invalid param"
                  << std::endl
                  << "when == " << when << " smaller than 0" << std::endl;
    }
    mTimerQueue.addTimer(Callback, when, interval);
}

bool EventManager::isLocalThread() {
    return mThreadId == CurrentThread::gettid();
}

void EventManager::runInLoop(const EventManager::Callback &callback) {
    if(isLocalThread()){
        callback();
    }else{
        std::cout << "EventManager::runInLoop==>"
                  << "called by foreign thread:" << CurrentThread::gettid() << std::endl;
        queueInLoop(callback);
    }
}

void EventManager::queueInLoop(const EventManager::Callback &callback) {
    {
        MutexLockGuard localGuard(mMutexLock);
        mCallbackQueue.push_back(callback);
    }

    if(mEventfdCallbackProcessing || !isLocalThread())
        wakeup();
}

void EventManager::handleRead() {
    uint64_t many;
    read(mEventFd, &many, sizeof many);
    std::cout << "EventManager::handleRead==>"
              << "Eventfd found " << many << " item(s)" << std::endl;
}

void EventManager::wakeup() {
    uint64_t one = 1;
    write(mEventFd, &one, sizeof one);
    std::cout << "EventManager::wakeup==>"
              << "send a signal to wakeup" << std::endl;
}

void EventManager::eventfdCallbackProcess() {
    std::cout << "EventManager::eventfdCallbackProcess==>"
              << std::endl;
    mEventfdCallbackProcessing = true;
    CallbackList callbackList;
    {
        MutexLockGuard localGuard(mMutexLock);
        mCallbackQueue.swap(callbackList);
    }

    for(int i=0; i<callbackList.size(); i++){
        callbackList[i]();
    }
    mEventfdCallbackProcessing = false;
}
