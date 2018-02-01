//
// Created by borelset on 1/18/18.
//

#include <Utils/Logger/LoggerManager.h>
#include "SockAddr.h"
#include "TCPClient.h"
#include "netUtils.h"

NetModule::TCPClient::TCPClient(ep::EventManager* eventManager,
                                SockAddr& addr,
                                const std::string& name) :
        mEventManager(eventManager),
        mServerAddr(addr),
        mName(name),
        mConnector(mEventManager, addr),
        mConnectionIndex(0),
        mConnected(false)
{
    mConnector.setConnectionCallback(
            std::bind(&TCPClient::newConnection, this, std::placeholders::_1)
    );
}

void NetModule::TCPClient::connect() {
    Log::LogInfo << "NNetModule::TCPClient::connect==>"
                 << "call connector connect" << Log::endl;
    mConnector.start();
}

void NetModule::TCPClient::disconnect() {
    Log::LogInfo << "NNetModule::TCPClient::disconnect==>"
                 << "call connector disconnect" << Log::endl;
    mTCPConnectionPtr->shutDown();
}

void NetModule::TCPClient::setConnectionCallback(const ConnectionCallback& callback) {
    mConnectionCallback = callback;
}

void NetModule::TCPClient::setMessageCallback(const MessageCallback& callback) {
    mMessageCallback = callback;
}

void NetModule::TCPClient::newConnection(int fd) {
    Log::LogInfo << "NetModule::TCPClient::newConnection==>"
                 << "new TCPConnection" << Log::endl;
    std::string connName = mName + "_Connection";
    mTCPConnectionPtr.reset(
            new TCPConnection(connName, fd,
                              SockAddr(getLocalAddr(fd)),
                              mServerAddr,
                              mEventManager)
    );
    mTCPConnectionPtr->setConnectionCallback(mConnectionCallback);
    mTCPConnectionPtr->setMessageCallback(mMessageCallback);
    mTCPConnectionPtr->setCloseCallback(
            std::bind(&TCPClient::removeConnection, this, std::placeholders::_1)
    );
    mTCPConnectionPtr->connectionEstablish();
    mConnected = true;
}

void NetModule::TCPClient::removeConnection(std::weak_ptr<NetModule::TCPConnection> connPtr) {
    mConnected = false;
    Log::LogInfo << "NNetModule::TCPClient::newConnection==>"
                 << "remove TCPConnection" << Log::endl;
    std::shared_ptr<TCPConnection> sharedPtr = connPtr.lock();
    mEventManager->runInLoop(
            std::bind(&TCPConnection::connectionDestroy, sharedPtr)
    );
    mTCPConnectionPtr.reset();
}

void NetModule::TCPClient::send(std::string& message) {
    Log::LogInfo << "NNetModule::TCPClient::send==>"
                 << "send message" << Log::endl;
    if(!mConnected){
        Log::LogWarning << "NNetModule::TCPClient::send==>"
                        << "Not connected" << Log::endl;
        return;
    }
    if(mEventManager->isLocalThread()){
        mTCPConnectionPtr->send(message);
    }else{
        mEventManager->runInLoop(
                std::bind(&TCPConnection::send, mTCPConnectionPtr, message)
        );
    }
}

bool NetModule::TCPClient::isConnected() {
    return mConnected;
}
