//
// Created by borelset on 1/18/18.
//

#include "../ep/EventManager.h"
#include "Connector.h"
#include "../Utils/Logger/LoggerManager.h"

NetModule::Connector::Connector(ep::EventManager *eventmanager, SockAddr &addr):
        mEventManager(eventmanager),
        mServerAddr(addr),
        mChannelPtr(new ep::Channel(mEventManager, mSocketPtr->getSocket())),
        mRetryTime(DEFAULT_RETRY_TIME)
{
    mChannelPtr->setWriteCallback(
            std::bind(&Connector::writeHandle, this)
    );
    mChannelPtr->enableWrite();
}

NetModule::Connector::~Connector() {

}

void NetModule::Connector::setConnectionCallback(const NetModule::Connector::ConnectionCallback &callback) {
    mConnectionCallback = callback;
}

void NetModule::Connector::start() {
    mEventManager->runInLoop(
            std::bind(&Connector::startInLoop, this)
    );
}

void NetModule::Connector::stop() {

}

void NetModule::Connector::writeHandle() {
    mConnectionCallback(mSocketPtr->getSocket());
}

void NetModule::Connector::startInLoop() {
    mSocketPtr = std::unique_ptr<Socket>();
    int ret = mSocketPtr->connect(mServerAddr);
    switch (ret){  //All errnos in man connect
        case 0:
        case EINPROGRESS:
        case EINTR:
        case EISCONN:
            active();
            break;
        case EAGAIN:
        case EADDRINUSE:
        case EADDRNOTAVAIL:
        case ECONNREFUSED:
        case ENETUNREACH:
        case ETIMEDOUT:
            retry();
            break;
        case EACCES:
        case EPERM:
        case EAFNOSUPPORT:
        case EALREADY:
        case EBADF:
        case EFAULT:
        case ENOTSOCK:
        case EPROTOTYPE:
        default:
            fail();
    }
}

void NetModule::Connector::stopInLoop() {

}

void NetModule::Connector::active() {
    setState(ConnectorState::Connecting);
    mChannelPtr.reset(new ep::Channel(mEventManager, mSocketPtr->getSocket()));
    mChannelPtr->setWriteCallback(
            std::bind(&Connector::writeHandle, this)
    );
    mChannelPtr->enableWrite();
}

void NetModule::Connector::setState(NetModule::Connector::ConnectorState connectorState) {
    mConnectState = connectorState;
}

void NetModule::Connector::retry() {
    mSocketPtr = nullptr;
    setState(ConnectorState::Disconnected);
    mEventManager->runAfter(
            std::bind(&Connector::startInLoop, this),
            mRetryTime,
            0
    );
    mRetryTime = std::min(mRetryTime*2, MAX_RETRY_TIME);
}

void NetModule::Connector::fail() {
    mSocketPtr = nullptr;
}
