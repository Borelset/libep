//
// Created by borelset on 1/18/18.
//

#include "../ep/EventManager.h"
#include "Connector.h"
#include "../Utils/Logger/LoggerManager.h"
#include "netUtils.h"

NetModule::Connector::Connector(ep::EventManager *eventmanager, SockAddr &addr):
        DEFAULT_RETRY_TIME(rand()%10+1),
        mEventManager(eventmanager),
        mServerAddr(addr),
        mChannelPtr(),
        mRetryTime(DEFAULT_RETRY_TIME)
{

}

NetModule::Connector::~Connector() {

}

void NetModule::Connector::setConnectionCallback(const NetModule::Connector::ConnectionCallback &callback) {
    mConnectionCallback = callback;
}

void NetModule::Connector::start() {
    Log::LogInfo << "NetModule::Connector::start==>"
                 << "Connector start" << Log::endl;
    mEventManager->runInLoop(
            std::bind(&Connector::startInLoop, this)
    );
}

void NetModule::Connector::stop() {
    Log::LogInfo << "NetModule::Connector::stop==>"
                 << "Connector stop" << Log::endl;
    mEventManager->runInLoop(
            std::bind(&Connector::stopInLoop, this)
    );
    mEventManager->stopTimer(mRetryTimer);
}

void NetModule::Connector::writeHandle() {
    Log::LogInfo << "NetModule::Connector::writeHandle==>"
                 << "test success" << Log::endl;
    if(mConnectState == ConnectorState::Connecting){
        mChannelPtr->disableAll();
        mEventManager->removeChannel(mChannelPtr.get());
        int sockfd = mChannelPtr->getFd();
        mChannelPtr.reset();

        int error = getSockError(sockfd);
        if(error != 0){
            retry();
        }else if(isSelfConnect(sockfd)){
            retry();
        }else{
            setState(ConnectorState::connected);
            mConnectionCallback(sockfd);
        }
    }
}

void NetModule::Connector::startInLoop() {
    Log::LogInfo << "NetModule::Connector::startInLoop==>"
                 << "connect" << Log::endl;
    mSocketPtr = std::unique_ptr<Socket>(new Socket());
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
    Log::LogInfo << "NetModule::Connector::stopInLoop==>"
                 << "stop" << Log::endl;
    setState(ConnectorState::Disconnected);
    mChannelPtr->disableAll();
    mEventManager->removeChannel(mChannelPtr.get());
    mSocketPtr.reset();
}

void NetModule::Connector::active() {
    Log::LogInfo << "NetModule::Connector::active==>"
                 << "connect successful" << Log::endl;
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
    Log::LogInfo << "NetModule::Connector::retry==>"
                 << "connect try again" << Log::endl;
    mSocketPtr.reset();
    setState(ConnectorState::Disconnected);
    mRetryTimer = mEventManager->runAfter(
            std::bind(&Connector::startInLoop, this),
            mRetryTime,
            0
    );
    mRetryTime = std::min(mRetryTime*2, MAX_RETRY_TIME);
}

void NetModule::Connector::fail() {
    Log::LogInfo << "NetModule::Connector::fail==>"
                 << "fail in connect" << Log::endl;
    mSocketPtr.reset();
}