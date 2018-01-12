//
// Created by borelset on 1/10/18.
//

#ifndef NETMODEL_PROCESSOR_H
#define NETMODEL_PROCESSOR_H

#include <functional>
#include "Utils.h"

class EventManager;

namespace ep{
    class Channel{
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
    private:
        EventCallback mReadCallback;
        EventCallback mWriteCallback;
        EventCallback mErrorCallback;

        int mEventFd;
        int mEvent;
        int mRevent;
        ChannelStatus mStatus;
        EventManager* mEventManager;

        void update();
    };
}




#endif //NETMODEL_PROCESSOR_H
