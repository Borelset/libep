#include <iostream>
#include <fcntl.h>
#include <zconf.h>
#include "ep/Channel.h"
#include "ep/EpollHandler.h"
#include "ep/EventManager.h"
#include "ep/TimerQueue.h"
#include "ep/CurrentThread.h"
#include <pthread.h>
#include <sys/timerfd.h>
#include <string.h>
#include <syscall.h>
#include "ep/Thread.h"
#include "ep/EventManagerThread.h"

int fd;

using namespace ep;

void testRead(){
    //std::string buffer;
    //std::cin >> buffer;
    //std::cout << "hello read! " << buffer << std::endl;
    std::cout << "hello read! " << std::endl;
}
void testWrite(){
    std::cout << "hello write!" << std::endl;
}

void* thread(void* em){
    sleep(5);
    EventManager* eventManager = (EventManager*)em;
    //eventManager->runAfter(testWrite, 5, 0);
    std::cout << "thread:" << CurrentThread::gettid() << " compare:" << eventManager->isLocalThread() << std::endl;
    std::function<void()> test = std::bind(&EventManager::runAfter, eventManager, testRead,  1, 0);
    eventManager->runInLoop(test);
}


int main() {
    std::cout << getpid() << std::endl;

    EventManagerThread eventManagerThread;
    eventManagerThread.start();
    pthread_t pthread;
    std::cout << "main:" << CurrentThread::gettid() << std::endl;
    std::function<void()> testcallback = std::bind(&EventManager::runAfter, eventManagerThread.getEventManagerHandler(), testRead, 1, 1);
    std::function<void()> testcallback2 = std::bind(&EventManager::runInLoop, eventManagerThread.getEventManagerHandler(), testcallback);
    ep::Thread testThread(testcallback2);
    testThread.run();
    //pthread_create(&pthread, nullptr, thread, &eventManager);

    //eventManager.runAfter(testRead, 5, 0);
    //eventManager.runAfter(testRead, 10, 0);
    //eventManager.runAfter(testRead, 15, 0);
    //eventManager.runAt(testRead, 10+getTime(), 1);
    sleep(1000);
    return 0;

}