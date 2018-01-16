//
// Created by borelset on 1/10/18.
//

#ifndef LIBEP_EP_CHANNEL_H
#define LIBEP_EP_CHANNEL_H

#include <functional>
#include "epollUtils.h"
#include "../Utils/noncopyable.h"

namespace ep{
    class EventManager;

    class Channel : Utils::noncopyable{
    public:
        typedef std::function<void()> EventCallback;
        Channel(EventManager*, int);
        ~Channel();
        void process();

        void setReadCallback(const EventCallback&);
        void setWritrCallback(const EventCallback&);
        void setErrorCallback(const EventCallback&);

        void enableRead();
        void enableWrite();
        void disbleRead();
        void disableWrite();
        void setEvent(int);
        void setStatus(ChannelStatus);
        ChannelStatus getStatus();
        void setRevent(int);
        int getFd();
        bool isNoneEvent();
        int getEvent();
        void disableAll();
    private:
        EventCallback mReadCallback;
        EventCallback mWriteCallback;
        EventCallback mErrorCallback;
        EventCallback mCloseCallback;

        int mEventFd;
        int mEvent;
        int mRevent;
        ChannelStatus mStatus;
        EventManager* mEventManager;

        void update();
    };
}




#endif //LIBEP_EP_CHANNEL_H
