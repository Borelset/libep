//
// Created by borelset on 1/10/18.
//

#ifndef NETMODEL_EVENTMANAGER_H
#define NETMODEL_EVENTMANAGER_H

#include <bits/unique_ptr.h>
#include <vector>
#include <functional>
#include "TimerQueue.h"

class Channel;
class EpollHandler;

class EventManager{
public:
    typedef std::vector<Channel*> ChannelList;
    EventManager(pid_t ThreadId);
    ~EventManager();
    void loop();
    void updateChannel(Channel*);
    void setQuit();
    void runAt(std::function<void()> Callback, time_t when, int interval);
    void runAfter(std::function<void()> Callback, time_t when, int interval);
    bool isNativeThread(pid_t threadId);
    void testExist();
private:
    std::unique_ptr<EpollHandler> mEpollHandlerPtr;
    bool mQuit;
    TimerQueue mTimerQueue;
    const pid_t mThreadId;
};

#endif //NETMODEL_EVENTMANAGER_H
