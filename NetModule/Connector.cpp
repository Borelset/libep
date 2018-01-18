//
// Created by borelset on 1/18/18.
//

#include "../ep/EventManager.h"
#include "Connector.h"

NetModule::Connector::Connector(ep::EventManager *eventmanager, SockAddr &addr):
        mEventManager(eventmanager),
        mServerAddr(addr),
        mChannelPtr(new ep::Channel(mEventManager, mSocketPtr->getSocket()))
{
    mChannelPtr->setWriteCallback(
            std::bind(&Connector::writeHandle, this)
    );
    mChannelPtr->enableWrite();
}

NetModule::Connector::~Connector() {

}

void NetModule::Connector::setConnectionCallback(NetModule::Connector::ConnectionCallback &callback) {
    mConnectionCallback = callback;
}

void NetModule::Connector::start() {
    mSocketPtr->connect(mServerAddr);
}

void NetModule::Connector::restart() {
    mSocketPtr = std::unique_ptr<Socket>(new Socket());
    mChannelPtr = std::unique_ptr<ep::Channel>(
            new ep::Channel(mEventManager, mSocketPtr->getSocket())
    );
    mChannelPtr->setWriteCallback(
            std::bind(&Connector::writeHandle, this)
    );
    mChannelPtr->enableWrite();
    start();
}

void NetModule::Connector::stop() {

}

void NetModule::Connector::writeHandle() {
    int opt = -1;
    socklen_t length = sizeof opt;
    if(getsockopt(mSocketPtr->getSocket(), SOL_SOCKET, SO_ERROR, &opt, &length) != 0){
        std::cout << "NetModule::Connector::writeHandle==>"
                  << "socket error" << std::endl;
    }
    mConnectionCallback(mSocketPtr->getSocket());
}
