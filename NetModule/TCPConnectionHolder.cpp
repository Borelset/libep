//
// Created by borelset on 1/18/18.
//

#include <memory>
#include "TCPConnectionHolder.h"
#include "TCPConnection.h"

NetModule::TCPConnectionHolder::TCPConnectionHolder(TCPConnection* connPtr, ep::EventManager* eventManager) :
        mTCPConnectionPtr(connPtr),
        mEventManager(eventManager)
{

}

NetModule::TCPConnectionHolder::~TCPConnectionHolder() {
    mEventManager->runInLoop(
            std::bind(&TCPConnection::forceClose, mTCPConnectionPtr)
    );
}

std::weak_ptr<NetModule::TCPConnection> NetModule::TCPConnectionHolder::getTCPConnectionPtr() {
    return mTCPConnectionPtr;
}

NetModule::TCPConnectionHolder::TCPConnectionHolder() {
    mTCPConnectionPtr = nullptr;
}
