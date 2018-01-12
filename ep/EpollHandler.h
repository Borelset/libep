//
// Created by borelset on 1/10/18.
//

#ifndef NETMODEL_EPOLLER_H
#define NETMODEL_EPOLLER_H

#include <ctime>
#include <vector>
#include <map>
#include "Channel.h"

namespace ep{
    class EpollHandler{
    public:
        typedef std::vector<Channel*> ChannelList;
        typedef std::map<int, Channel*> ChannelMap;
        typedef std::vector<struct epoll_event> EpollEventList;
        EpollHandler();
        ~EpollHandler();
        time_t epoll(int, ChannelList*);
        void updateChannel(Channel *);
    private:
        int mEpollFd;
        ChannelMap mChannelMap;
        EpollEventList mEpollEvent;

        void activeEventCollector(int, ChannelList*);
        void epollCtlManager(int, Channel*);
    };
}


#endif //NETMODEL_EPOLLER_H
