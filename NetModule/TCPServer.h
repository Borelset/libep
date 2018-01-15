//
// Created by borelset on 1/15/18.
//

#ifndef LIBEP_TCPSERVER_H
#define LIBEP_TCPSERVER_H

#include <map>
#include "../ep/EventManager.h"
#include "Acceptor.h"
#include "TCPConnection.h"

namespace NetModule{
    class TCPServer{
    public:
        typedef std::map<std::string, std::shared_ptr<TCPConnection>> ConnectionMap;
        TCPServer(int port);
        ~TCPServer();
        void start();
        void setListenCallback(const std::function<void(int, SockAddr)>&);
        void setMessageCallback(const std::funcion);
    private:
        ep::EventManager mEventManager;
        Acceptor mAcceptor;
        ConnectionMap mConnectionMap;
    };
}

#endif //LIBEP_TCPSERVER_H
