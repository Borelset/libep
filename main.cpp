#include <iostream>
#include <zconf.h>
#include "ep/Channel.h"
#include "ep/EpollHandler.h"
#include "ep/EventManager.h"
#include <arpa/inet.h>
#include "NetModule/SockAddr.h"
#include "NetModule/Acceptor.h"

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

    //NetModule test
    std::shared_ptr<ep::EventManager> eventManager(new EventManager);
    std::weak_ptr<EventManager> testptr = eventManager;
    NetModule::Acceptor mAcceptor(testptr, 9981);
    mAcceptor.setListonCallback(callback);
    mAcceptor.listen();
    eventManager.get()->loop();

    return 0;

}