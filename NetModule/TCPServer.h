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
    class TCPServer : Utils::noncopyable{
    public:
        typedef std::map<std::string, std::shared_ptr<TCPConnection>> ConnectionMap;
        typedef std::function<void(std::weak_ptr<TCPConnection>)> ConnectionCallback;
        typedef std::function<void(std::weak_ptr<TCPConnection>, Utils::Buffer*, ::time_t)> MessageCallback;

        explicit TCPServer(int port);
        ~TCPServer();
        void start();
        void setConnectionCallback(const ConnectionCallback&);
        void setMessageCallback(const MessageCallback&);
        void removeConnection(std::shared_ptr<TCPConnection>);
    private:
        ep::EventManager mEventManager;
        Acceptor mAcceptor;
        ConnectionMap mConnectionMap;
        const std::string mConnName;
        int mNextConnId;
        SockAddr mLocalAddr;
        MessageCallback mMessageCallback;
        ConnectionCallback mConnectionCallback;

        void newConnection(int fd, SockAddr& addr);
    };
}

#endif //LIBEP_TCPSERVER_H
