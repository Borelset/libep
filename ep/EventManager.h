//
// Created by borelset on 1/10/18.
//

#ifndef NETMODEL_EVENTMANAGER_H
#define NETMODEL_EVENTMANAGER_H

#include <bits/unique_ptr.h>
#include <vector>
#include <functional>
#include "TimerQueue.h"
#include "Mutex.h"


namespace ep{
    class Channel;
    class EpollHandler;

    class EventManager{
    public:
        typedef std::vector<Channel*> ChannelList;
        typedef std::function<void()> Callback;
        typedef std::vector<Callback> CallbackList;
        EventManager();
        ~EventManager();
        void loop();
        void updateChannel(Channel*);
        void setQuit();
        void runAt(std::function<void()> Callback, time_t when, int interval);
        void runAfter(std::function<void()> Callback, time_t when, int interval);
        bool isLocalThread();
        void runInLoop(const Callback& callback);
    private:
        std::unique_ptr<EpollHandler> mEpollHandlerPtr;
        bool mQuit;
        TimerQueue mTimerQueue;
        const pid_t mThreadId;
        CallbackList mCallbackQueue;
        MutexLock mMutexLock;
        int mEventFd;
        Channel mWakeupChannel;
        bool mEventfdCallbackProcessing;

        void queueInLoop(const Callback& callback);
        void handleRead();
        void eventfdCallbackProcess();
        void wakeup();
    };
}


#endif //NETMODEL_EVENTMANAGER_H
