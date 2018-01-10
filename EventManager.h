//
// Created by borelset on 1/10/18.
//

#ifndef NETMODEL_EVENTMANAGER_H
#define NETMODEL_EVENTMANAGER_H

#include <bits/unique_ptr.h>
#include <vector>

class Channel;
class EpollHandler;

class EventManager{
public:
    typedef std::vector<Channel*> ChannelList;
    EventManager();
    ~EventManager();
    void loop();
    void updateChannel(Channel*);
    void setQuit();
private:
    std::unique_ptr<EpollHandler> mEpollHandlerPtr;
    bool mQuit;
};

#endif //NETMODEL_EVENTMANAGER_H
