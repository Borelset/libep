//
// Created by borelset on 1/15/18.
//

#ifndef LIBEP_NETMODULE_ACCEPTOR_H
#define LIBEP_NETMODULE_ACCEPTOR_H

#include <memory>
#include "Socket.h"
#include "../ep/EventManager.h"

namespace NetModule{
    class Acceptor{
    public:
        typedef std::function<void(int, SockAddr&)> ConnectionCallback;
        Acceptor(std::weak_ptr<ep::EventManager> eventManager, int port);
        void setListonCallback(const ConnectionCallback& connectionCallback);
        bool isListening() const;
        void listen();
    private:
        std::weak_ptr<ep::EventManager> mEventManager;
        Socket mSocket;
        ep::Channel mListenChannel;
        ConnectionCallback mListenCallback;
        bool mListening;

        void handleAccept();
    };
}

#endif //LIBEP_NETMODULE_ACCEPTOR_H
