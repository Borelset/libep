//
// Created by borelset on 1/18/18.
//

#ifndef LIBEP_CONNECTOR_H
#define LIBEP_CONNECTOR_H

#include "../Utils/noncopyable.h"
#include "../ep/EventManager.h"
#include "SockAddr.h"
#include "Socket.h"

namespace NetModule{
    class Connector : Utils::noncopyable{
    public:
        typedef std::function<void(int)> ConnectionCallback;
        Connector(ep::EventManager* eventmanager, SockAddr& addr);
        ~Connector();
        void setConnectionCallback(ConnectionCallback& callback);
        void writeHandle();
        void start();
        void restart();
        void stop();
    private:
        ConnectionCallback mConnectionCallback;
        ep::EventManager* mEventManager;
        std::unique_ptr<Socket> mSocketPtr;
        SockAddr mServerAddr;
        std::unique_ptr<ep::Channel> mChannelPtr;
    };
}

#endif //LIBEP_CONNECTOR_H
