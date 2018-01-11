//
// Created by borelset on 1/10/18.
//

#ifndef NETMODEL_UTILS_H
#define NETMODEL_UTILS_H

#include <sys/epoll.h>
#include <iostream>
#include <ostream>
#include <syscall.h>
#include <zconf.h>


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

static time_t getTime(){
    time_t time_;
    time(&time_);
    return time_;
}

static pid_t getTid(){
    return syscall(SYS_gettid);
}

#endif //NETMODEL_UTILS_H
