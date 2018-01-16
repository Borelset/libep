//
// Created by borelset on 1/16/18.
//

#include <memory>
#include "SockAddr.h"
#include "../ep/EventManager.h"
#include "TCPConnection.h"

NetModule::TCPConnection::TCPConnection(std::string name, int fd,
                                        NetModule::SockAddr &localAddr,
                                        NetModule::SockAddr &peerAddr,
                                        ep::EventManager* eventmanager):
                            mEventManagerPtr(eventmanager),
                            mName(name),
                            mChannel(new ep::Channel(eventmanager, fd)),
                            mFd(fd),
                            mLocalSockAddr(localAddr),
                            mPeerSockAddr(peerAddr),
                            mState(TCSConnecting)
{
    mChannel.get()->setReadCallback(std::bind(&TCPConnection::readHandle, this));
    mChannel.get()->enableRead();
}

void NetModule::TCPConnection::setState(NetModule::TCPConnection::TCPConnectionState state) {
    mState = state;
}

void NetModule::TCPConnection::readHandle() {
    char buf[65535];
    ssize_t n = ::read(mFd.getFd(), buf, sizeof buf);
    if(n > 0){
        mMessageCallback(shared_from_this(), buf, n);
    }else if(n == 0){
        closeHandle();
    }else{
        errorHanle();
    }

}

void NetModule::TCPConnection::setMessageCallback(const NetModule::TCPConnection::MessageCallback &messageCallback) {
    mMessageCallback = messageCallback;
}

void NetModule::TCPConnection::setConnectionCallback(
        const NetModule::TCPConnection::ConnectionCallback &connectionCallback) {
    mConnectionCallback = connectionCallback;
}

void NetModule::TCPConnection::closeHandle() {
    if(!mEventManagerPtr->isLocalThread()){
        return;
    }
    mChannel->disableAll();
    mCloseCallback(shared_from_this());
}

void NetModule::TCPConnection::errorHanle() {

}

void NetModule::TCPConnection::setCloseCallback(const CloseCallback & callback) {
    mCloseCallback = callback;
}

std::string NetModule::TCPConnection::getName() {
    return mName;
}

void NetModule::TCPConnection::connectionDestroy() {
    setState(TCSDisconnected);
    mChannel->disableAll();
    mConnectionCallback(*this);
    mEventManagerPtr->removeChannel();
}


