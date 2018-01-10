#include <iostream>
#include <fcntl.h>
#include <zconf.h>
#include "Channel.h"
#include "EpollHandler.h"
#include "EventManager.h"
#include <pthread.h>

int fd;

void testRead(){
    std::string buffer;
    std::cin >> buffer;
    std::cout << "hello read! " << buffer << std::endl;
}
void testWrite(){
    std::cout << "hello write!" << std::endl;
}

void* thread(void*){
    sleep(5);
    close(fd);
    std::cout << fd << "closed" << std::endl;
}



int main() {
    pthread_t test_pid;
    EventManager eventManager;

    Channel testChannel(&eventManager, STDIN_FILENO);

    testChannel.setReadCallback(testRead);

    testChannel.setWritrCallback(testWrite);

    testChannel.enableRead();

    eventManager.loop();
    return 0;
}