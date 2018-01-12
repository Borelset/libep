//
// Created by borelset on 1/10/18.
//
#include <string.h>
#include <zconf.h>
#include <iostream>
#include "EpollHandler.h"

using namespace ep;

EpollHandler::EpollHandler() {
    mEpollFd = epoll_create(EpollSize);
    mEpollEvent.resize(1);
}

EpollHandler::~EpollHandler() {
    close(mEpollFd);
}

time_t EpollHandler::epoll(int timeout, EpollHandler::ChannelList* channelList) {
    int eventNum = epoll_wait(mEpollFd,
                              mEpollEvent.data(),
                              (int)mEpollEvent.size(),
                              -1);
    if(eventNum > 0){
        activeEventCollector(eventNum, channelList);
        if(eventNum == (int)mEpollEvent.size()){
            mEpollEvent.resize(mEpollEvent.size() * 2);
        }
    }

    return 0;
}

void EpollHandler::updateChannel(Channel * channel) {
    ChannelStatus status = channel->getStatus();
    if(status == ChannelStatusNew || status == ChannelStatusDeleted){
        int fd = channel->getFd();
        if(status == ChannelStatusNew){
            mChannelMap[fd] = channel;
        }else{

        }
        channel->setStatus(ChannelStatusAdded);
        epollCtlManager(EPOLL_CTL_ADD, channel);
    }else{
        if(channel->isNoneEvent()){
            epollCtlManager(EPOLL_CTL_DEL, channel);
            channel->setStatus(ChannelStatusDeleted);
        }else{
            epollCtlManager(EPOLL_CTL_MOD, channel);
        }
    }
}

void EpollHandler::activeEventCollector(int num, EpollHandler::ChannelList* channelList) {
    for(int i=0; i<num; i++){
        Channel* channel = (Channel*)mEpollEvent[i].data.ptr;
        channel->setRevent(mEpollEvent[i].events);
        channelList->push_back(channel);
    }
}

void EpollHandler::epollCtlManager(int operate, Channel * channel) {
    struct epoll_event event;
    bzero(&event, sizeof(struct epoll_event));
    event.events = (uint32_t)channel->getEvent();
    event.data.ptr = channel;
    epoll_ctl(mEpollFd, operate, channel->getFd(), &event);
}
