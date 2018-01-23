//
// Created by borelset on 1/18/18.
//

#ifndef LIBEP_TCPCONNECTIONHOLDER_H
#define LIBEP_TCPCONNECTIONHOLDER_H

#include <memory>
#include "../Utils/noncopyable.h"
#include "../ep/EventManager.h"

namespace NetModule{
    class TCPConnection;

    class TCPConnectionHolder{
    public:
        TCPConnectionHolder();
        explicit TCPConnectionHolder(TCPConnection* connPtr, ep::EventManager*);
        ~TCPConnectionHolder();
        std::weak_ptr<TCPConnection> getTCPConnectionPtr();

    private:
        ep::EventManager* mEventManager;
        std::shared_ptr<TCPConnection> mTCPConnectionPtr;
    };
}

#endif //LIBEP_TCPCONNECTIONHOLDER_H
