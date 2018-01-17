#include <iostream>
#include <zconf.h>
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
              << " Receive " << readBuffer->getReadble() << " bytes at " << time << std::endl;
    std::cout << "Message:" << readBuffer->getContent() << std::endl;
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
    tcpserverTest();

    return 0;

}