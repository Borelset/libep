//
// Created by borelset on 1/10/18.
//

#ifndef NETMODEL_UTILS_H
#define NETMODEL_UTILS_H

#include <sys/epoll.h>
#include <iostream>
#include <ostream>


const int EpollSize = 200;

enum ChannelStatus{
    ChannelStatusNew,
    ChannelStatusAdded,
    ChannelStatusDeleted
};

enum EpollEvent{
    EpollNoneEvent = 0,
    EpollReadEvent = EPOLLIN,
    EpollWriteEvent = EPOLLOUT,
    EpollErrorEvent = EPOLLERR
};

#endif //NETMODEL_UTILS_H
