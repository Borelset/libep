//
// Created by borelset on 1/15/18.
//

#include "Socket.h"
#include "Acceptor.h"
#include "netUtils.h"
#include "../Utils/Logger/LoggerManager.h"

NetModule::Acceptor::Acceptor(ep::EventManager* eventManager, int port):
        mEventManager(eventManager),
        mSocket(),
        mListenChannel(mEventManager, mSocket.getSocket()),
        mListening(false)
{
    mSocket.bindAddr("0.0.0.0", port);
    mListenChannel.setReadCallback(std::bind(&Acceptor::handleAccept, this));
    Log::LogInfo << "NetModule::Acceptor::Acceptor=>>"
                 << "Construction" << Log::endl;
}

void NetModule::Acceptor::listen() {
    Log::LogInfo << "NetModule::Acceptor::listen=>>"
                 << "Start to listen" << Log::endl;
    mListening = true;
    mSocket.listen();
    mListenChannel.enableRead();
}

void NetModule::Acceptor::handleAccept() {
    if(!mEventManager->isLocalThread()){
        Log::LogError << "NetModule::Acceptor::handleAccept=>>"
                      << "called out of local thread and force exit" << Log::endl;
        return;
    }
    SockAddr sockAddr;
    int accfd = mSocket.accept(&sockAddr.getAddr());
    if(accfd > 0){
        Log::LogError << "NetModule::Acceptor::handleAccept=>>"
                      << "accept connection" << Log::endl;
        mListenCallback(accfd, sockAddr);
    }else{
        Log::LogError << "NetModule::Acceptor::handleAccept=>>"
                      << "fail in accept connection" << Log::endl;
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
