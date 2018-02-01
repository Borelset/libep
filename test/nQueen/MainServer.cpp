//
// Created by borelset on 2/1/18.
//
#include "nQueenServerLogic.h"
#include <iostream>
#include <functional>
#include <NetModule/TCPConnection.h>
#include <NetModule/TCPServer.h>
#include <cstring>

using namespace NetModule;
using namespace std;

nQueenServerLogic* QSL;

void messageCallback(weak_ptr<TCPConnection> tcpc, time_t time){
    shared_ptr<TCPConnection> sharedPtr = tcpc.lock();
    string received = sharedPtr->getReadBuffer()->getContent();
    string message;
    nQueenServerLogic::ServerState serverState =
            nQueenServerLogic::statePrasor(received);
    switch (serverState){
        case nQueenServerLogic::ServerState::Ready:
            message = QSL->readyCallback(received, sharedPtr->getName());
            break;
        case nQueenServerLogic::ServerState::Task:
            message = QSL->taskCallback(received, sharedPtr->getName());
            break;
        case nQueenServerLogic::ServerState::Result:
            message = QSL->resultCallback(received, sharedPtr->getName());
            break;
        case nQueenServerLogic::ServerState::Unknown:
        default:
            message = QSL->errorCallback(received, sharedPtr->getName());
    }

    if(message.size()>0){
        sharedPtr->send(message);
    }
}

Utils::MutexLock gLock;
Utils::Condition gCondition(gLock);

TCPServer* serverPtr;

void threadProxy(int port, int n){
    std::cout << port << endl;
    TCPServer mServer(port);
    nQueenServerLogic qsl(n);
    serverPtr = &mServer;
    QSL = &qsl;
    gCondition.notify();
    mServer.setTimingWheelCircle(0);
    mServer.setMessageCallback(messageCallback);
    mServer.start();
}

int main(int argc, char** argv){
    int port = atoi(argv[1]);
    int n = atoi(argv[2]);

    Utils::Thread mThread(
            std::bind(threadProxy, port, n)
    );
    mThread.run();
    {
        Utils::MutexLockGuard localLock(gLock);
        gCondition.wait();
    }
    string input;
    while(strcmp(input.c_str(), "exit")){
        cout << "\n$";
        cin >> input;
        if(!strcmp(input.c_str(), "start")){
            cout << "There is " << serverPtr->connCount() << " slave server at service" << endl;
            QSL->start();
        }
    }
}

