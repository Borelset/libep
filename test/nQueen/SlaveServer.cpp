//
// Created by borelset on 2/1/18.
//

#include "nQueenServerLogic.h"
#include <iostream>
#include <functional>
#include <NetModule/TCPConnection.h>
#include <NetModule/TCPServer.h>
#include <cstring>
#include <NetModule/TCPClient.h>
#include "ep/EventManager.h"

using namespace NetModule;
using namespace std;

nQueenServerLogic* QSL;

void messageCallback(weak_ptr<TCPConnection> tcpc, time_t time){
    std::cout << "messageCallback" << endl;
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

int main(int argc, char** argv){
    cout << "start to connect" << endl;
    ep::EventManagerThread eventManagerThread;
    eventManagerThread.start();
    ep::EventManager* eventManager = eventManagerThread.getEventManagerHandler();

    QSL = new nQueenServerLogic(0);

    SockAddr serverAddr("127.0.0.1", 9999);
    TCPClient mClient(eventManager, serverAddr, "slave");
    mClient.setMessageCallback(messageCallback);
    mClient.connect();
    while(!mClient.isConnected());
    string message("Ready");
    mClient.send(message);
    sleep(100);
}

