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
    constexpr static time_t MAX_RETRY_TIME = 30;

    class Connector : Utils::noncopyable{
    public:
        enum class ConnectorState{
            Connecting,
            Disconnected,
            connected
        };
        typedef std::function<void(int)> ConnectionCallback;

        Connector(ep::EventManager* eventmanager, SockAddr& addr);
        ~Connector();
        void setConnectionCallback(const ConnectionCallback& callback);
        void writeHandle();
        void start();
        void stop();
        void setState(ConnectorState);
    private:
        time_t DEFAULT_RETRY_TIME;
        ConnectionCallback mConnectionCallback;
        ep::EventManager* mEventManager;
        std::unique_ptr<Socket> mSocketPtr;
        SockAddr mServerAddr;
        std::unique_ptr<ep::Channel> mChannelPtr;
        ConnectorState mConnectState;
        time_t mRetryTime;
        std::weak_ptr<ep::Timer> mRetryTimer;

        void startInLoop();
        void stopInLoop();
        void active();
        void retry();
        void fail();
    };
}

#endif //LIBEP_CONNECTOR_H
