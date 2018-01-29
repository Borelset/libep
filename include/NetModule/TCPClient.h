//
// Created by borelset on 1/18/18.
//

#ifndef LIBEP_TCPCLIENT_H
#define LIBEP_TCPCLIENT_H

#include "../Utils/noncopyable.h"
#include "Connector.h"
#include "TCPConnection.h"

namespace NetModule{
    class TCPClient : Utils::noncopyable{
    public:
        using TCPConnectionPtr = std::shared_ptr<TCPConnection>;
        using ConnectionCallback = std::function<void(std::weak_ptr<TCPConnection>)>;
        using MessageCallback = std::function<void(std::weak_ptr<TCPConnection>, ::time_t)>;

        TCPClient(ep::EventManager*, SockAddr&, const std::string&);
        void connect();
        void disconnect();
        void setConnectionCallback(const ConnectionCallback&);
        void setMessageCallback(const MessageCallback&);
        void send(std::string&);
        bool isConnected();
    private:
        ep::EventManager* mEventManager;
        std::string mName;
        SockAddr mServerAddr;
        Connector mConnector;
        TCPConnectionPtr mTCPConnectionPtr;
        ConnectionCallback mConnectionCallback;
        MessageCallback mMessageCallback;
        int mConnectionIndex;
        bool mConnected;

        void newConnection(int);
        void removeConnection(std::weak_ptr<TCPConnection>);
    };
}

#endif //LIBEP_TCPCLIENT_H
