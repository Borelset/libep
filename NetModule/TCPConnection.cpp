//
// Created by borelset on 1/16/18.
//

#include <memory>
#include <arpa/inet.h>
#include "SockAddr.h"
#include "../ep/EventManager.h"
#include "TCPConnection.h"
#include "../Utils/Utils.h"
#include "../Utils/Logger/LoggerManager.h"
#include "../Utils/CurrentThread.h"

NetModule::TCPConnection::TCPConnection(std::string name, int fd,
                                        const NetModule::SockAddr &localAddr,
                                        const NetModule::SockAddr &peerAddr,
                                        ep::EventManager* eventmanager):
                            mEventManagerPtr(eventmanager),
                            mName(name),
                            mChannel(new ep::Channel(eventmanager, fd)),
                            mFd(fd),
                            mSocket(new Socket(mFd, peerAddr)),
                            mLocalSockAddr(localAddr),
                            mPeerSockAddr(peerAddr),
                            mState(TCSConnecting)
{
    Log::LogInfo << "NetModule::TCPConnection::TCPConnection==>"
                 << "Construction" << Log::endl;
    mChannel->setReadCallback(std::bind(&TCPConnection::readHandle, this));
    mChannel->setWriteCallback(std::bind(&TCPConnection::writeHandle, this));
    mChannel->setErrorCallback(nullptr);
    mSocket->setKeepAlive(true);
}

void NetModule::TCPConnection::setState(NetModule::TCPConnection::TCPConnectionState state) {
    Log::LogInfo << "NetModule::TCPConnection::setState==>"
                 << "Set " << state << Log::endl;
    mState = state;
}

void NetModule::TCPConnection::readHandle() {
    int n = mReadBuffer.readFromFd(mFd.getFd());
    Log::LogInfo << "NetModule::TCPConnection::readHandle==>"
                 << "read " << n << " character(s)" << Log::endl;
    if(n > 0){
        if(mMessageCallback) mMessageCallback(shared_from_this(), Utils::getTime());
        if(mRefreshCallback) mRefreshCallback(mName);
    }else if(n == 0){
        Log::LogInfo << "NetModule::TCPConnection::readHandle==>"
                     << "Ready to close" << Log::endl;
        closeHandle();
    }else{
        errorHandle();
    }

}

void NetModule::TCPConnection::setMessageCallback(const NetModule::TCPConnection::MessageCallback &messageCallback) {
    Log::LogInfo << "NetModule::TCPConnection::setMessageCallback==>"
                 << "set new MessageCallback" << Log::endl;
    mMessageCallback = messageCallback;
}

void NetModule::TCPConnection::setConnectionCallback(
        const NetModule::TCPConnection::ConnectionCallback &connectionCallback) {
    Log::LogInfo << "NetModule::TCPConnection::setConnectionCallback==>"
                 << "set new ConnectionCallback" << Log::endl;
    mConnectionCallback = connectionCallback;
}

void NetModule::TCPConnection::closeHandle() {
    Log::LogInfo << "NetModule::TCPConnection::closeHandle==>"
                 << "close" << Log::endl;
    setState(TCSDisconnecting);
    if(!mEventManagerPtr->isLocalThread()){
        Log::LogError << "NetModule::TCPConnection::closeHandle==>>"
                     << "this should be called in local thread" << Log::endl;
        return;
    }
    mChannel->disableAll();
    mCloseCallback(shared_from_this());
}

void NetModule::TCPConnection::errorHandle() {
    Log::LogInfo << "NetModule::TCPConnection::errorHandle==>"
                 << "error" << Log::endl;
}

void NetModule::TCPConnection::setCloseCallback(const CloseCallback & callback) {
    Log::LogInfo << "NetModule::TCPConnection::setCloseCallback==>"
                 << "set new CloseCallback" << Log::endl;
    mCloseCallback = callback;
}

std::string NetModule::TCPConnection::getName() {
    return mName;
}

void NetModule::TCPConnection::connectionDestroy() {
    Log::LogInfo << "NetModule::TCPConnection::connectionDestroy==>"
                 << mName << " destroying.." << Log::endl;
    setState(TCSDisconnected);
    mChannel->disableAll();
    if(mConnectionCallback) mConnectionCallback(shared_from_this());
    mEventManagerPtr->removeChannel(mChannel.get());
}

NetModule::TCPConnection::~TCPConnection() {
    Log::LogInfo << "NetModule::TCPConnection::~TCPConnection==>"
                 << mName << " releasing.." << Log::endl;
}

std::string NetModule::TCPConnection::getIp() {
    return std::string(inet_ntoa(mPeerSockAddr.getAddr().sin_addr));
}

int NetModule::TCPConnection::getPort() {
    return mPeerSockAddr.getAddr().sin_port;
}

void NetModule::TCPConnection::send(const std::string& message) {
    Log::LogInfo << "NetModule::TCPConnection::send==>"
                 << "Send message" << Log::endl;
    if(mState == TCSConnected){
        if(mEventManagerPtr->isLocalThread()){
            sendInLoop(message);
        }else{
            mEventManagerPtr->runInLoop(
                    std::bind(&TCPConnection::sendInLoop, this, message)
            );
        }
    }else{
        std::cout << "not connected";
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
    Log::LogInfo << "NetModule::TCPConnection::sendInLoop==>"
                 << "Send message" << Log::endl;
    ssize_t writeChars = 0;
    if(!mChannel->isWriting() && mWriteBuffer.getReadble() == 0){
        writeChars = ::write(mFd.getFd(), message.c_str(), message.size());
        if(writeChars < 0){
            Log::LogError << "NetModule::TCPConnection::sendInLoop==>"
                          << "Error in write" << Log::endl;
            writeChars = 0;
        }
    }
    Log::LogInfo << "NetModule::TCPConnection::sendInLoop==>"
                 << "Write " << writeChars << " character(s)" << Log::endl;
    if(writeChars < message.size()){
        Log::LogInfo << "NetModule::TCPConnection::sendInLoop==>"
                     << "Some message left" << Log::endl;
        mWriteBuffer.writeBuffer(
                (char*)message.data() + writeChars, message.size()-writeChars);
        if(!mChannel->isWriting()){
            mChannel->enableWrite();
        }
    }
}

void NetModule::TCPConnection::shutDownInLoop() {
    if(!mEventManagerPtr->isLocalThread()){
        Log::LogWarning << "NetModule::TCPConnection::shutDownInLoop==>"
                        << "not in local thread" << Log::endl;
    }
    if(!mChannel->isWriting()){
        mSocket->shutDown();
    }
}

void NetModule::TCPConnection::writeHandle() {
    if(!mEventManagerPtr->isLocalThread()) {
        Log::LogWarning << "NetModule::TCPConnection::writeHandle==>"
                        << "not in local thread" << Log::endl;
    }
    if(mChannel->isWriting()){
        ssize_t n = ::write(mFd.getFd(), mWriteBuffer.readPoint(), mWriteBuffer.getReadble());
        if(n>0){
            mWriteBuffer.setForward(n);
            if(mWriteBuffer.getReadble() == 0){
                mChannel->disableWrite();
                if(mState == TCSDisconnecting)
                    shutDownInLoop();
            }else{
                Log::LogInfo << "NetModule::TCPConnection::writeHandle==>"
                             << mName << ":more message need to write" << Log::endl;
            }
        }else{
            Log::LogError << "NetModule::TCPConnection::writeHandle==>"
                          << mName << ": n=" << n << " Error in write" << Log::endl;
        }
    }else{
        Log::LogError << "NetModule::TCPConnection::writeHandle==>>"
                     << mName << ":Not allow to write" << Log::endl;
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

Utils::Buffer *NetModule::TCPConnection::getReadBuffer() {
    return &mReadBuffer;
}

Utils::Buffer *NetModule::TCPConnection::getWriteBuffer() {
    return &mWriteBuffer;
}

void NetModule::TCPConnection::forceClose() {
    closeHandle();
}

void NetModule::TCPConnection::setRefreshCallback(const NetModule::TCPConnection::RefreshCallback &callback) {
    mRefreshCallback = callback;
}

