//
// Created by borelset on 1/15/18.
//

#ifndef LIBEP_TCPSERVER_H
#define LIBEP_TCPSERVER_H

#include <map>
#include "../ep/EventManager.h"
#include "Acceptor.h"
#include "TCPConnection.h"
#include "../ep/EventManagerThreadPool.h"
#include "TimingWheel.h"

namespace NetModule{
    class TCPConnectionHolder;

    constexpr static int DEFAULT_TIMINGWHEEL = 10;

    class TCPServer : Utils::noncopyable{
    public:
        //typedef std::map<std::string, std::shared_ptr<TCPConnection>> ConnectionMap;
        typedef std::map<std::string, std::weak_ptr<TCPConnectionHolder>> ConnectionMap;
        typedef std::function<void(std::weak_ptr<TCPConnection>)> ConnectionCallback;
        typedef std::function<void(std::weak_ptr<TCPConnection>, ::time_t)> MessageCallback;

        explicit TCPServer(int port);
        ~TCPServer();
        void start();
        void setConnectionCallback(const ConnectionCallback&);
        void setMessageCallback(const MessageCallback&);
        void removeConnection(std::weak_ptr<TCPConnection>);
        void removeConnectionInOwnerManager(std::weak_ptr<TCPConnection>);
        void refreshConnection(std::string&);
        void setTimingWheelCircle(int);
    private:
        ep::EventManager mEventManager;
        ep::EventManagerThreadPool mEventManagerThreadPool;
        Acceptor mAcceptor;
        ConnectionMap mConnectionMap;
        TimingWheel mTimingWheel;
        const std::string mConnName;
        int mNextConnId;
        SockAddr mLocalAddr;
        MessageCallback mMessageCallback;
        ConnectionCallback mConnectionCallback;
        std::weak_ptr<ep::Timer> mTimerWheelHandle;

        void newConnection(int fd, SockAddr& addr);
    };
}

#endif //LIBEP_TCPSERVER_H
