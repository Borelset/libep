#include "ep/Channel.h"
#include "ep/EpollHandler.h"
#include "ep/EventManager.h"
#include "NetModule/SockAddr.h"
#include "NetModule/Acceptor.h"
#include "NetModule/TCPServer.h"

std::string message1 = "hello everyone";

void messageCallback(std::weak_ptr<NetModule::TCPConnection> tcpc, time_t time){
    auto sharedPtr = tcpc.lock();
    if(sharedPtr == nullptr) return;

    printf("Receive %lu character(s) at %lu\n",
           sharedPtr->getReadBuffer()->getReadble(),
           time );
    printf("Message: %s\n",
           sharedPtr->getReadBuffer()->getContent().c_str() );
    sharedPtr->send(message1);
}

void connectionCallback(std::weak_ptr<NetModule::TCPConnection> tcpc){
    auto sharedPtr = tcpc.lock();
    if(sharedPtr == nullptr) return;

    if(sharedPtr->isConnected()){
        printf("New connection %s from %s:%d\n",
               sharedPtr->getName().c_str(),
               sharedPtr->getIp().c_str(),
               sharedPtr->getPort() );
    }else{
        printf("Connection %s shutdown\n",
               sharedPtr->getName().c_str() );
    }
}

int main() {
    NetModule::TCPServer tcpServer(9981);
    tcpServer.setConnectionCallback(connectionCallback);
    tcpServer.setMessageCallback(messageCallback);
    tcpServer.start();

    return 0;
}
