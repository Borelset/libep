//
// Created by borelset on 1/15/18.
//

#include "Socket.h"
#include "Acceptor.h"

NetModule::Acceptor::Acceptor(std::weak_ptr<ep::EventManager> eventManager, int port):
        mEventManager(eventManager),
        mSocket(),
        mListenChannel(mEventManager.lock().get(), mSocket.getSocket()),
        mListening(false)
{
    mSocket.bindAddr("127.0.0.1", port);
    mListenChannel.setReadCallback(std::bind(&Acceptor::handleAccept, this));
}

void NetModule::Acceptor::listen() {
    mListening = true;
    mSocket.listen();
    mListenChannel.enableRead();
}

void NetModule::Acceptor::handleAccept() {
    if(!mEventManager.lock().get()->isLocalThread()){
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
