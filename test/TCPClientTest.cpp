#include <NetModule/TCPClient.h>
#include <ep/EventManagerThread.h>
#include "ep/EventManager.h"

void testMessageCallback(std::weak_ptr<NetModule::TCPConnection> conn, time_t time){
    auto sharedPtr = conn.lock();
    if(sharedPtr == nullptr) return;

    printf("Receive %lu character(s) at %lu\n",
           sharedPtr->getReadBuffer()->getReadble(),
           time );
    printf("Message: %s\n",
           sharedPtr->getReadBuffer()->getContent().c_str() );
    printf("waiting for server auto close\n");
}

void testConnectionCallback(std::weak_ptr<NetModule::TCPConnection> tcpc){
    auto sharedPtr = tcpc.lock();
    if(sharedPtr == nullptr) return;

    if(sharedPtr->isConnected()){
        printf("New connection %s connect to %s:%d\n",
               sharedPtr->getName().c_str(),
               sharedPtr->getIp().c_str(),
               sharedPtr->getPort() );
    }else{
        printf("Connection %s shutdown\n",
               sharedPtr->getName().c_str() );
    }
}

std::string message = "hello i`m client!";

int main(){
    ep::EventManagerThread eventManagerThread;
    eventManagerThread.start();
    ep::EventManager* handler = nullptr;
    while( ( handler = eventManagerThread.getEventManagerHandler())
           == nullptr );
    NetModule::SockAddr serverAddr("127.0.0.1", 9981);
    NetModule::TCPClient tcpClient(handler,
                                   serverAddr,
                                   "testClient");
    tcpClient.setMessageCallback(testMessageCallback);
    tcpClient.setConnectionCallback(testConnectionCallback);
    tcpClient.connect();

    while(!tcpClient.isConnected());
    tcpClient.send(message);
    sleep(12);
}