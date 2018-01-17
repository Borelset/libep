//
// Created by borelset on 1/15/18.
//

#include <arpa/inet.h>
#include "TCPServer.h"

NetModule::TCPServer::TCPServer(int port):
        mAcceptor(&mEventManager, port),
        mConnName("Connection"),
        mNextConnId(0),
        mLocalAddr(mAcceptor.getLocalAddr())
{
    mAcceptor.setListonCallback(std::bind(&TCPServer::newConnection, this, std::placeholders::_1, std::placeholders::_2));
    std::cout << "NetModule::TCPServer::TCPServer==>"
              << "Construction" << std::endl;
}

NetModule::TCPServer::~TCPServer() {
    std::cout << "NetModule::TCPServer::~TCPServer==>"
              << "Destruction" << std::endl;
}

void NetModule::TCPServer::start() {
    std::cout << "NetModule::TCPServer::start==>"
              << "Start" << std::endl;
    mAcceptor.listen();
    mEventManager.loop();
}

void NetModule::TCPServer::setConnectionCallback(const ConnectionCallback& callback) {
    std::cout << "NetModule::TCPServer::setConnectionCallback==>"
              << "Set new connection callback" << std::endl;
    mConnectionCallback = callback;
}

void NetModule::TCPServer::newConnection(int fd, NetModule::SockAddr &addr) {
    char buffer[32];
    snprintf(buffer, sizeof buffer, "#%d", mNextConnId);
    mNextConnId++;
    std::string connName = mConnName + buffer;
    std::cout << "NetModule::TCPServer::newConnection==>"
              << "\n\tnew connection "<< connName << std::endl
              << "\tfrom " << inet_ntoa(addr.getAddr().sin_addr) << ":" << addr.getAddr().sin_port << std::endl;
    std::shared_ptr<TCPConnection> connPtr(
            new TCPConnection(connName, fd, mLocalAddr, addr, &mEventManager));
    mConnectionMap[connName] = connPtr;
    connPtr->setMessageCallback(mMessageCallback);
    connPtr->setConnectionCallback(mConnectionCallback);
    connPtr->setCloseCallback(std::bind(&TCPServer::removeConnection, this, std::placeholders::_1));
}

void NetModule::TCPServer::setMessageCallback(const MessageCallback & callback) {
    std::cout << "NetModule::TCPServer::setMessageCallback==>"
              << "Set new Message callback" << std::endl;
    mMessageCallback = callback;
}

void NetModule::TCPServer::removeConnection(std::shared_ptr<NetModule::TCPConnection> connPtr) {
    size_t n = mConnectionMap.erase(connPtr->getName());
    std::cout << "NetModule::TCPServer::removeConnection==>"
              << "mConnectionMap earse " << n << " item(s)" << std::endl;
    if(n != 1){
        std::cout << "NetModule::TCPServer::removeConnection==>"
                  << "Wrong number things erased from ConnectionMap. They have same Connection Name but it should not." << std::endl;
    }
    mEventManager.runInLoop(std::bind(&TCPConnection::connectionDestroy, connPtr));
}
