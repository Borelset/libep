#include <iostream>
#include <zconf.h>
#include "ep/Channel.h"
#include "ep/EpollHandler.h"
#include "ep/EventManager.h"
#include <arpa/inet.h>
#include "NetModule/SockAddr.h"
#include "NetModule/Acceptor.h"
#include "NetModule/TCPServer.h"

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

void messagecallback(std::weak_ptr<NetModule::TCPConnection> tcpc, char* buf, int n){
    std::cout << n << " bytes" << std::endl;
    std::cout << buf << std::endl;
}


int main() {
    std::cout << getpid() << std::endl;

    /*
    //ep test
    EventManagerThread eventManagerThread;
    eventManagerThread.start();
    pthread_t pthread;
    std::cout << "main:" << Utils::CurrentThread::gettid() << std::endl;
    std::function<void()> testcallback =
            std::bind(&EventManager::runAfter,
                      eventManagerThread.getEventManagerHandler(),
                      testRead, 1, 1);
    Utils::Thread testThread(testcallback);
    testThread.run();
    sleep(1000);
     */

    /*
    //Acceptor test
    std::shared_ptr<ep::EventManager> eventManager(new EventManager);
    std::weak_ptr<EventManager> testptr = eventManager;
    NetModule::Acceptor mAcceptor(testptr, 9981);
    mAcceptor.setListonCallback(callback);
    mAcceptor.listen();
    eventManager.get()->loop();
     */

    //TCPserver test
    NetModule::TCPServer tcpServer(9981);
    tcpServer.setMessageCallback(messagecallback);
    tcpServer.start();

    return 0;

}