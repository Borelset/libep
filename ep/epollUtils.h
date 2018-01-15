//
// Created by borelset on 1/10/18.
//

#ifndef LIBEP_EP_EPOLLUTILS_H
#define LIBEP_EP_EPOLLUTILS_H

#include <sys/epoll.h>
#include <iostream>
#include <ostream>
#include <syscall.h>
#include <zconf.h>

namespace ep{
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
}



#endif //LIBEP_EP_EPOLLUTILS_H
