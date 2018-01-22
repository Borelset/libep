//
// Created by borelset on 1/15/18.
//

#ifndef LIBEP_TCPCONNECTION_H
#define LIBEP_TCPCONNECTION_H

#include "Socket.h"
#include "../Utils/Buffer.h"

namespace NetModule{
    class TCPConnection : Utils::noncopyable,
                          public std::enable_shared_from_this<TCPConnection>
    {
    public:
        typedef std::function<void(std::weak_ptr<TCPConnection>)> ConnectionCallback;
        typedef std::function<void(std::weak_ptr<TCPConnection>, ::time_t)> MessageCallback;
        typedef std::function<void(std::shared_ptr<TCPConnection>)> CloseCallback;
        TCPConnection(std::string name,
                      int fd,
                      SockAddr& localAddr,
                      SockAddr& peerAddr,
                      ep::EventManager* eventmanager);
        ~TCPConnection();
        void setMessageCallback(const MessageCallback& messageCallback);
        void setConnectionCallback(const ConnectionCallback& connectionCallback);
        void setCloseCallback(const CloseCallback& callback);
        void send(const std::string& message);
        void shutDown();
        std::string getName();
        std::string getIp();
        int getPort();
        void connectionEstablish();
        void connectionDestroy();
        bool isConnected();
        ep::EventManager* getManager();
        Utils::Buffer* getReadBuffer();
        Utils::Buffer* getWriteBuffer();
    private:
        enum TCPConnectionState{
            TCSConnecting,
            TCSConnected,
            TCSDisconnecting,
            TCSDisconnected
        };
        void setState(TCPConnectionState state);
        void readHandle();
        void writeHandle();
        void closeHandle();
        void errorHandle();
        void sendInLoop(const std::string& message);
        void shutDownInLoop();

        ep::EventManager* mEventManagerPtr;
        std::string mName;
        TCPConnectionState mState;
        std::unique_ptr<ep::Channel> mChannel;
        Utils::FD mFd;
        std::unique_ptr<Socket> mSocket;
        SockAddr mLocalSockAddr;
        SockAddr mPeerSockAddr;
        ConnectionCallback mConnectionCallback;
        MessageCallback mMessageCallback;
        CloseCallback mCloseCallback;
        Utils::Buffer mReadBuffer;
        Utils::Buffer mWriteBuffer;
    };
}

#endif //LIBEP_TCPCONNECTION_H
