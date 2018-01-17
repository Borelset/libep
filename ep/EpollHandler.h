//
// Created by borelset on 1/10/18.
//

#ifndef LIBEP_EP_EPOLLER_H
#define LIBEP_EP_EPOLLER_H

#include <ctime>
#include <vector>
#include <map>
#include "Channel.h"
#include "../Utils/FD.h"

namespace ep{
    class EpollHandler : Utils::noncopyable{
    public:
        typedef std::vector<Channel*> ChannelList;
        typedef std::map<int, Channel*> ChannelMap;
        typedef std::vector<struct epoll_event> EpollEventList;
        EpollHandler();
        ~EpollHandler();
        time_t epoll(int, ChannelList*);
        void updateChannel(Channel *);
        void removeChannel(Channel *);
    private:
        Utils::FD mEpollFd;
        ChannelMap mChannelMap;
        EpollEventList mEpollEvent;

        void activeEventCollector(int, ChannelList*);
        void epollCtlManager(int, Channel*);
    };
}


#endif //LIBEP_EP_EPOLLER_H
