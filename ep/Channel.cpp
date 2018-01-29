//
// Created by borelset on 1/10/18.
//

#include "Channel.h"
#include "EventManager.h"
#include "../Utils/Logger/LoggerManager.h"

using namespace ep;

void Channel::process() {
    if(mRevent & EpollReadEvent)
        if(mReadCallback){
            mReadCallback();
        }
    if(mRevent & EpollWriteEvent)
        if(mWriteCallback){
            mWriteCallback();
        }
    if(mRevent & EpollErrorEvent)
        if(mErrorCallback){
            mErrorCallback();
        }
}

void Channel::enableRead() {
    mEvent |= EpollReadEvent;
    update();
}

void Channel::enableWrite() {
    mEvent |= EpollWriteEvent;
    update();
}

void Channel::disbleRead() {
    mEvent &= ~EpollReadEvent;
    update();
}

void Channel::disableWrite() {
    mEvent &= ~EpollWriteEvent;
    update();
}

void Channel::setRevent(int event) {
    mRevent = event;
}


void Channel::setStatus(ChannelStatus status) {
    mStatus = status;
}

ChannelStatus Channel::getStatus() {
    return mStatus;
}

void Channel::update() {
    mEventManager->updateChannel(this);
}

int Channel::getFd() {
    return mEventFd;
}

bool Channel::isNoneEvent() {
    return mEvent == 0;
}

int Channel::getEvent() {
    return mEvent;
}

void Channel::setReadCallback(const Channel::EventCallback & eventCallback) {
    mReadCallback = eventCallback;
}

void Channel::setWriteCallback(const EventCallback &eventCallback) {
    mWriteCallback = eventCallback;
}

void Channel::setErrorCallback(const Channel::EventCallback & eventCallback) {
    mErrorCallback = eventCallback;
}

Channel::Channel(EventManager* eventManager, int fd):
        mEventManager(eventManager),
        mEventFd(fd),
        mEvent(EpollNoneEvent),
        mRevent(EpollNoneEvent),
        mStatus(ChannelStatusNew)
{
    mEvent |= EpollCloseEvent;
    update();
}

Channel::~Channel() {
    Log::LogInfo << "ep::Channel::~Channel==>"
              << "Channel with fd:" << mEventFd << " releasing.." << Log::endl;
}

void Channel::setEvent(int event) {
    mEvent = event;
    update();
}

void Channel::disableAll() {
    mEvent = EpollNoneEvent;
    update();
}

bool Channel::isWriting() {
    return static_cast<bool>(mEvent & EpollWriteEvent);
}

