//
// Created by borelset on 1/16/18.
//

#include <memory>
#include <arpa/inet.h>
#include "SockAddr.h"
#include "../ep/EventManager.h"
#include "TCPConnection.h"
#include "../Utils/Utils.h"

NetModule::TCPConnection::TCPConnection(std::string name, int fd,
                                        NetModule::SockAddr &localAddr,
                                        NetModule::SockAddr &peerAddr,
                                        ep::EventManager* eventmanager):
                            mEventManagerPtr(eventmanager),
                            mName(name),
                            mChannel(new ep::Channel(eventmanager, fd)),
                            mFd(fd),
                            mSocket(new Socket(mFd)),
                            mLocalSockAddr(localAddr),
                            mPeerSockAddr(peerAddr),
                            mState(TCSConnecting)
{
    std::cout << "NetModule::TCPConnection::TCPConnection==>"
              << "Construction" << std::endl;
    mChannel->setReadCallback(std::bind(&TCPConnection::readHandle, this));
    mChannel->setWriteCallback(std::bind(&TCPConnection::writeHandle, this));
    mChannel->setErrorCallback(std::bind(&TCPConnection::errorHandle, this));
    mSocket->setKeepAlive(true);
}

void NetModule::TCPConnection::setState(NetModule::TCPConnection::TCPConnectionState state) {
    std::cout << "NetModule::TCPConnection::setState==>"
              << "Set " << state << std::endl;
    mState = state;
}

void NetModule::TCPConnection::readHandle() {
    int n = mReadBuffer.readFromFd(mFd.getFd());
    std::cout << "NetModule::TCPConnection::readHandle==>"
              << "read " << n << " character(s)" << std::endl;
    if(n > 0){
        if(mMessageCallback) mMessageCallback(shared_from_this(), &mReadBuffer, Utils::getTime());
    }else if(n == 0){
        std::cout << "NetModule::TCPConnection::readHandle==>"
                  << "Ready to close" << std::endl;
        closeHandle();
    }else{
        errorHandle();
    }

}

void NetModule::TCPConnection::setMessageCallback(const NetModule::TCPConnection::MessageCallback &messageCallback) {
    std::cout << "NetModule::TCPConnection::setMessageCallback==>"
              << "set new MessageCallback" << std::endl;
    mMessageCallback = messageCallback;
}

void NetModule::TCPConnection::setConnectionCallback(
        const NetModule::TCPConnection::ConnectionCallback &connectionCallback) {
    std::cout << "NetModule::TCPConnection::setConnectionCallback==>"
              << "set new ConnectionCallback" << std::endl;
    mConnectionCallback = connectionCallback;
}

void NetModule::TCPConnection::closeHandle() {
    std::cout << "NetModule::TCPConnection::closeHandle==>"
              << "close" << std::endl;
    if(!mEventManagerPtr->isLocalThread()){
        std::cout << "NetModule::TCPConnection::closeHandle==>>"
                  << "this should be called in local thread" << std::endl;
        return;
    }
    mChannel->disableAll();
    mCloseCallback(shared_from_this());
}

void NetModule::TCPConnection::errorHandle() {
    std::cout << "NetModule::TCPConnection::errorHandle==>"
              << "error" << std::endl;
}

void NetModule::TCPConnection::setCloseCallback(const CloseCallback & callback) {
    std::cout << "NetModule::TCPConnection::setCloseCallback==>"
              << "set new CloseCallback" << std::endl;
    mCloseCallback = callback;
}

std::string NetModule::TCPConnection::getName() {
    return mName;
}

void NetModule::TCPConnection::connectionDestroy() {
    std::cout << "NetModule::TCPConnection::connectionDestroy==>"
              << mName << " destroying.." << std::endl;
    setState(TCSDisconnected);
    mChannel->disableAll();
    if(mConnectionCallback) mConnectionCallback(shared_from_this());
    mEventManagerPtr->removeChannel(mChannel.get());
}

NetModule::TCPConnection::~TCPConnection() {
    std::cout << "NetModule::TCPConnection::~TCPConnection==>"
              << mName << " releasing.." << std::endl;
}

std::string NetModule::TCPConnection::getIp() {
    return std::string(inet_ntoa(mPeerSockAddr.getAddr().sin_addr));
}

int NetModule::TCPConnection::getPort() {
    return mPeerSockAddr.getAddr().sin_port;
}

void NetModule::TCPConnection::send(const std::string& message) {
    if(mState == TCSConnected){
        if(mEventManagerPtr->isLocalThread()){
            sendInLoop(message);
        }else{
            mEventManagerPtr->runInLoop(
                    std::bind(&TCPConnection::sendInLoop, this, message)
            );
        }
    }
}

void NetModule::TCPConnection::shutDown() {
    if(mState == TCSConnected){
        setState(TCSDisconnecting);
        mEventManagerPtr->runInLoop(
                std::bind(&TCPConnection::shutDownInLoop, this)
        );
    }
}

void NetModule::TCPConnection::sendInLoop(const std::string& message) {
    ssize_t writeChars = 0;
    if(!mChannel->isWriting() && mWriteBuffer.getReadble() == 0){
        writeChars = ::write(mFd.getFd(), message.c_str(), message.size());
        if(writeChars < 0){
            std::cout << "NetModule::TCPConnection::sendInLoop==>"
                      << "Error in write" << std::endl;
            writeChars = 0;
        }
    }
    if(writeChars < message.size()){
        mWriteBuffer.writeBuffer(
                (char*)message.data() + writeChars, message.size()-writeChars);
        if(!mChannel->isWriting()){
            mChannel->enableWrite();
        }
    }
}

void NetModule::TCPConnection::shutDownInLoop() {
    if(!mEventManagerPtr->isLocalThread()){
        std::cout << "NetModule::TCPConnection::shutDownInLoop==>"
                  << "not in local thread" << std::endl;
    }
    if(!mChannel->isWriting()){
        mSocket->shutDown();
    }
}

void NetModule::TCPConnection::writeHandle() {
    if(!mEventManagerPtr->isLocalThread()) {
        std::cout << "NetModule::TCPConnection::writeHandle==>"
                  << "not in local thread" << std::endl;
    }
    if(mChannel->isWriting()){
        ssize_t n = ::write(mFd.getFd(), mWriteBuffer.readPoint(), mWriteBuffer.getReadble());
        if(n>0){
            mWriteBuffer.setForward(n);
            if(mWriteBuffer.getReadble() == 0){
                mChannel->disableWrite();
                if(mState == TCSDisconnecting)
                    shutDown();
            }else{
                std::cout << "NetModule::TCPConnection::writeHandle==>"
                          << mName << ":more message need to write" << std::endl;
            }
        }else{
            std::cout << "NetModule::TCPConnection::writeHandle==>"
                      << mName << ": n=" << n << " Error in write" << std::endl;
        }
    }else{
        std::cout << "NetModule::TCPConnection::writeHandle==>>"
                  << mName << ":Not allow to write" << std::endl;
    }
}

void NetModule::TCPConnection::connectionEstablish() {
    mChannel->enableRead();
    mState = TCSConnected;
    if(mConnectionCallback) mConnectionCallback(shared_from_this());
}

bool NetModule::TCPConnection::isConnected() {
    return mState == TCSConnected;
}

ep::EventManager* NetModule::TCPConnection::getManager() {
    return mEventManagerPtr;
}

