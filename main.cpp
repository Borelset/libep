#include <iostream>
#include <unistd.h>
#include "ep/Channel.h"
#include "ep/EpollHandler.h"
#include "ep/EventManager.h"
#include <arpa/inet.h>
#include "NetModule/SockAddr.h"
#include "NetModule/Acceptor.h"
#include "NetModule/TCPServer.h"
#include "ep/EventManagerThread.h"
#include "Utils/CurrentThread.h"

int fd;

using namespace ep;

std::string message1 = "hello everyone";
std::string message2 = "bye!";

void testRead(){
    std::cout << "hello read! " << std::endl;
}
void testWrite(){
    std::cout << "hello write!" << std::endl;
}

void callback(int fd, NetModule::SockAddr addr){
    std::cout << "received!! fd:" << fd << " addr:" << inet_ntoa(addr.getAddr().sin_addr) << " " << addr.getAddr().sin_port << std::endl;
    char buffer[] = "hello!";
}

void messagecallback(std::weak_ptr<NetModule::TCPConnection> tcpc, Utils::Buffer* readBuffer, time_t time){
    std::cout << "Connection " << tcpc.lock()->getName() << " from " << tcpc.lock()->getIp() << ":" << tcpc.lock()->getPort()
              << " Receive " << readBuffer->getReadble() << " character(s) at " << time << std::endl;
    std::cout << "Message:" << readBuffer->getContent() << std::endl;
    tcpc.lock()->send(message1);
    tcpc.lock()->shutDown();
}

void connectioncallback(std::weak_ptr<NetModule::TCPConnection> tcpc){
    if(tcpc.lock()->isConnected()){
        std::cout << "Connection " << tcpc.lock()->getName() << " from " << tcpc.lock()->getIp() << ":" << tcpc.lock()->getPort() << std::endl;
        tcpc.lock()->send(message1);
        tcpc.lock()->send(message2);
        tcpc.lock()->shutDown();
    }else{
        std::cout << "Connection " << tcpc.lock()->getName() << " down"<< std::endl;
    }
}

void epTest(){
    ep::EventManagerThread eventManagerThread;
    eventManagerThread.start();
    std::cout << "main:" << Utils::CurrentThread::gettid() << std::endl;
    std::function<void()> testcallback =
            std::bind(&EventManager::runAfter,
                      eventManagerThread.getEventManagerHandler(),
                      testRead, 1, 1);
    Utils::Thread testThread(testcallback);
    testThread.run();
    sleep(1000);
}

void acceptorTest(){
    EventManager* eventManager = new EventManager;
    NetModule::Acceptor mAcceptor(eventManager, 9981);
    mAcceptor.setListonCallback(callback);
    mAcceptor.listen();
    eventManager->loop();
}

void tcpserverTest(){
    NetModule::TCPServer tcpServer(9981);
    tcpServer.setMessageCallback(messagecallback);
    tcpServer.start();
}


int main() {
    std::cout << Utils::CurrentThread::gettid() << std::endl;
    tcpserverTest();

    return 0;

}