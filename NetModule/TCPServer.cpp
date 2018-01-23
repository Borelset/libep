//
// Created by borelset on 1/15/18.
//

#include <arpa/inet.h>
#include "TCPServer.h"
#include "../Utils/Logger/LoggerManager.h"
#include "TCPConnectionHolder.h"

NetModule::TCPServer::TCPServer(int port):
        mAcceptor(&mEventManager, port),
        mTimingWheel(DEFAULT_TIMINGWHEEL),
        mConnName("Connection"),
        mNextConnId(0),
        mLocalAddr(mAcceptor.getLocalAddr()),
        mEventManagerThreadPool(&mEventManager)
{
    mAcceptor.setListonCallback(
            std::bind(&TCPServer::newConnection, this, std::placeholders::_1, std::placeholders::_2) );
    mEventManagerThreadPool.setThreadNum(10);
    Log::LogInfo << "NetModule::TCPServer::TCPServer==>"
                 << "Construction" << Log::endl;
    mEventManager.runAfter(
            std::bind(&TimingWheel::goAhead, &mTimingWheel),
            1,
            1
    );
}

NetModule::TCPServer::~TCPServer() {
    Log::LogInfo << "NetModule::TCPServer::~TCPServer==>"
                 << "Destruction" << Log::endl;
}

void NetModule::TCPServer::start() {
    Log::LogInfo << "NetModule::TCPServer::start==>"
                 << "Start" << Log::endl;
    mEventManagerThreadPool.start();
    mAcceptor.listen();
    printf("Server Started Listening port:%d", mAcceptor.getLocalAddr().sin_port);
    mEventManager.loop();
}

void NetModule::TCPServer::setConnectionCallback(const ConnectionCallback& callback) {
    Log::LogInfo << "NetModule::TCPServer::setConnectionCallback==>"
                 << "Set new connection callback" << Log::endl;
    mConnectionCallback = callback;
}

void NetModule::TCPServer::newConnection(int fd, NetModule::SockAddr &addr) {
    char buffer[32];
    snprintf(buffer, sizeof buffer, "#%d", mNextConnId);
    mNextConnId++;
    std::string connName = mConnName + buffer;
    Log::LogInfo << "NetModule::TCPServer::newConnection==>"
                 << "\n\tnew connection "<< connName << Log::endl
                 << "\tfrom " << inet_ntoa(addr.getAddr().sin_addr) << ":" << addr.getAddr().sin_port << Log::endl;
    ep::EventManager* ioManager = mEventManagerThreadPool.getNextManager();
    //std::shared_ptr<TCPConnection> connPtr(
    std::shared_ptr<TCPConnectionHolder> connHolderPtr(
            new TCPConnectionHolder(
                    new TCPConnection(connName, fd, mLocalAddr, addr, ioManager),
                    ioManager
            )
    );
    mConnectionMap[connName] = connHolderPtr;
    mTimingWheel.refresh(connHolderPtr);
    auto sharedPtr = connHolderPtr->getTCPConnectionPtr().lock();
    sharedPtr->setMessageCallback(mMessageCallback);
    sharedPtr->setConnectionCallback(mConnectionCallback);
    sharedPtr->setCloseCallback(
            std::bind(&TCPServer::removeConnection, this, std::placeholders::_1)
    );
    sharedPtr->setRefreshCallback(
            std::bind(&TCPServer::refreshConnection, this, std::placeholders::_1)
    );
    ioManager->runInLoop(
            std::bind(&TCPConnection::connectionEstablish, sharedPtr)
    );
}

void NetModule::TCPServer::setMessageCallback(const MessageCallback & callback) {
    Log::LogInfo << "NetModule::TCPServer::setMessageCallback==>"
                 << "Set new Message callback" << Log::endl;
    mMessageCallback = callback;
}

void NetModule::TCPServer::removeConnection(std::weak_ptr<NetModule::TCPConnection> connPtr) {
    Log::LogInfo << "NetModule::TCPServer::removeConnection==>"
                 << "removeConnection" << Log::endl;
    mEventManager.runInLoop(
            std::bind(&TCPServer::removeConnectionInOwnerManager, this, connPtr)
    );
}

void NetModule::TCPServer::removeConnectionInOwnerManager(std::weak_ptr<NetModule::TCPConnection> connPtr) {
    auto sharedPtr = connPtr.lock();
    if(sharedPtr == nullptr){
        Log::LogError << "NetModule::TCPServer::removeConnectionInOwnerManager==>"
                     << "Connection has been destructed.." << Log::endl;
        return;
    }
    size_t n = mConnectionMap.erase(sharedPtr->getName());
    Log::LogInfo << "NetModule::TCPServer::removeConnectionInOwnerManager==>"
                 << "mConnectionMap earse " << n << " item(s)" << Log::endl;
    if(n != 1){
        Log::LogInfo << "NetModule::TCPServer::removeConnectionInOwnerManager==>"
                     << "Wrong number things erased from ConnectionMap. They have same Connection Name but it should not." << Log::endl;
    }
    ep::EventManager* ioManager = sharedPtr->getManager();
    ioManager->runInLoop(
            std::bind(&TCPConnection::connectionDestroy, sharedPtr)
    );
}

void NetModule::TCPServer::refreshConnection(std::string & connName) {
    auto connHolderPtr = mConnectionMap[connName].lock();
    mTimingWheel.refresh(connHolderPtr);
}
