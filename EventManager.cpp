//
// Created by borelset on 1/10/18.
//

#include "EventManager.h"
#include "EpollHandler.h"

EventManager::EventManager():
        mEpollHandlerPtr(new EpollHandler())
{

}

EventManager::~EventManager() {

}

void EventManager::loop() {
    ChannelList wakedChannel;
    while(mQuit){
        wakedChannel.clear();
        mEpollHandlerPtr->epoll(100, &wakedChannel);
        for(ChannelList::iterator it = wakedChannel.begin();
                it != wakedChannel.end();
                it++){
            (*it)->process();
        }
    }
}

void EventManager::updateChannel(Channel * channel) {
    mEpollHandlerPtr->updateChannel(channel);
}

void EventManager::setQuit() {
    mQuit = true;
}
