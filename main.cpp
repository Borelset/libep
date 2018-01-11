#include <iostream>
#include <fcntl.h>
#include <zconf.h>
#include "Channel.h"
#include "EpollHandler.h"
#include "EventManager.h"
#include "TimerQueue.h"
#include <pthread.h>
#include <sys/timerfd.h>
#include <string.h>
#include <syscall.h>

int fd;

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
    __thread pid_t threadId = getTid();
    std::cout << "thread1:" << threadId << std::endl;
    EventManager* eventManager = (EventManager*)em;
    //eventManager->runAfter(testWrite, 5, 0);
    std::cout << "thread:" << eventManager->isNativeThread(threadId) << std::endl;
    std::cout << "thread::done" << std::endl;
}


int main() {
    std::cout << getpid() << std::endl;

    EventManager eventManager(getTid());
    pthread_t pthread;
    pthread_create(&pthread, nullptr, thread, &eventManager);
    sleep(5);
    //eventManager.runAfter(testRead, 5, 0);
    //eventManager.runAt(testRead, 10+getTime(), 0);
    //timerQueue.testChannelCallback();

    //eventManager.loop();
    return 0;

}