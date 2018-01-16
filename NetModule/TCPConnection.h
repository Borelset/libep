//
// Created by borelset on 1/15/18.
//

#ifndef LIBEP_TCPCONNECTION_H
#define LIBEP_TCPCONNECTION_H

#include "Socket.h"

namespace NetModule{
    class TCPConnection : Utils::noncopyable,
                          public std::enable_shared_from_this<TCPConnection>
    {
    public:
        typedef std::function<void(TCPConnection&)> ConnectionCallback;
        typedef std::function<void(std::weak_ptr<TCPConnection>, char*, int)> MessageCallback;
        typedef std::function<void(std::shared_ptr<TCPConnection>)> CloseCallback;
        TCPConnection(std::string name,
                      int fd,
                      SockAddr& localAddr,
                      SockAddr& peerAddr,
                      ep::EventManager* eventmanager);
        void setMessageCallback(const MessageCallback& messageCallback);
        void setConnectionCallback(const ConnectionCallback& connectionCallback);
        void setCloseCallback(const CloseCallback& callback);
        std::string getName();
        void connectionDestroy();
    private:
        enum TCPConnectionState{
            TCSConnecting,
            TCSConnected,
            TCSDisconnected
        };
        void setState(TCPConnectionState state);
        void readHandle();
        void closeHandle();
        void errorHanle();

        ep::EventManager* mEventManagerPtr;
        std::string mName;
        TCPConnectionState mState;
        std::unique_ptr<ep::Channel> mChannel;
        Utils::FD mFd;
        SockAddr mLocalSockAddr;
        SockAddr mPeerSockAddr;
        ConnectionCallback mConnectionCallback;
        MessageCallback mMessageCallback;
        CloseCallback mCloseCallback;
    };
}

#endif //LIBEP_TCPCONNECTION_H
