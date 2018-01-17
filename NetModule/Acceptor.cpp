//
// Created by borelset on 1/15/18.
//

#include "Socket.h"
#include "Acceptor.h"
#include "netUtils.h"

NetModule::Acceptor::Acceptor(ep::EventManager* eventManager, int port):
        mEventManager(eventManager),
        mSocket(),
        mListenChannel(mEventManager, mSocket.getSocket()),
        mListening(false)
{
    mSocket.bindAddr("127.0.0.1", port);
    mListenChannel.setReadCallback(std::bind(&Acceptor::handleAccept, this));
    std::cout << "NetModule::Acceptor::Acceptor=>>"
              << "Construction" << std::endl;
}

void NetModule::Acceptor::listen() {
    std::cout << "NetModule::Acceptor::listen=>>"
              << "Start to listen" << std::endl;
    mListening = true;
    mSocket.listen();
    mListenChannel.enableRead();
}

void NetModule::Acceptor::handleAccept() {
    if(!mEventManager->isLocalThread()){
        std::cout << "NetModule::Acceptor::handleAccept=>>"
                  << "called out of local thread and force exit" << std::endl;
        return;
    }
    SockAddr sockAddr;
    int accfd = mSocket.accept(&sockAddr.getAddr());
    if(accfd > 0){
        mListenCallback(accfd, sockAddr);
    }else{
        std::cout << "NetModule::Acceptor::handleAccept=>>"
                  << "fail in accept connection" << std::endl;
    }
}

void NetModule::Acceptor::setListonCallback(const NetModule::Acceptor::ConnectionCallback &connectionCallback) {
    mListenCallback = connectionCallback;
}

bool NetModule::Acceptor::isListening() const {
    return mListening;
}

struct sockaddr_in NetModule::Acceptor::getLocalAddr() {
    return NetModule::getLocalAddr(mSocket.getSocket());
}

NetModule::Acceptor::~Acceptor() {

}
