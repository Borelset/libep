//
// Created by borelset on 1/10/18.
//

#ifndef LIBEP_EP_EPOLLUTILS_H
#define LIBEP_EP_EPOLLUTILS_H

#include <sys/epoll.h>
#include <iostream>
#include <ostream>
#include <syscall.h>
#include <unistd.h>

namespace ep{
    constexpr static int EpollSize = 200;

    enum ChannelStatus{
        ChannelStatusNew,
        ChannelStatusAdded,
        ChannelStatusDeleted
    };

    enum EpollEvent{
        EpollNoneEvent = 0,
        EpollReadEvent = EPOLLIN | EPOLLPRI | EPOLLHUP,
        EpollWriteEvent = EPOLLOUT,
        EpollErrorEvent = EPOLLERR,
        EpollCloseEvent = EPOLLHUP
    };
}



#endif //LIBEP_EP_EPOLLUTILS_H
