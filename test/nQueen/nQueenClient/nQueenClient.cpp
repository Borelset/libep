//
// Created by borelset on 2/1/18.
//

#include "test/nQueen/nQueenClient/nQueenClientLogic.h"
#include <iostream>
#include <functional>
#include <NetModule/TCPConnection.h>
#include <NetModule/TCPServer.h>
#include <cstring>
#include <string>

using namespace NetModule;
using namespace std;

nQueenClientLogic* QCL;

void messageCallback(weak_ptr<TCPConnection> tcpc, time_t time){
    shared_ptr<TCPConnection> sharedPtr = tcpc.lock();
    string received = sharedPtr->getReadBuffer()->getContent();
    string message;
    nQueenClientLogic::ClientState serverState =
            nQueenClientLogic::stateParser(received);
    switch (serverState){
        case nQueenClientLogic::ClientState::Ready:
            message = QCL->readyCallback(received, sharedPtr->getName());
            break;
        case nQueenClientLogic::ClientState::Result:
            message = QCL->resultCallback(received, sharedPtr->getName());
            break;
        case nQueenClientLogic::ClientState::Unknown:
        default:
            message = QCL->errorCallback(received, sharedPtr->getName());
    }

    if(message.size()>0){
        sharedPtr->send(message);
    }
}

vector<NetModule::SockAddr> serverList = {
        {"127.0.0.1", 9001},
        {"127.0.0.1", 9002},
        {"127.0.0.1", 9003},
};

int main(){
    ep::EventManagerThread eventManagerThread;
    eventManagerThread.start();
    ep::EventManager* eventManager = eventManagerThread.getEventManagerHandler();
    if(eventManager == nullptr){
        cout << "something wrong..\nfail to run a thread" << endl;
    }
    nQueenClientLogic qcl(eventManager);
    qcl.setMessageCallback(messageCallback);
    QCL = &qcl;
    for(int i=0; i<serverList.size(); i++){
        qcl.setClient(serverList[i]);
    }

    string input;
    while(strcmp(input.c_str(), "exit")){
        cout << "$";
        getline(cin, input);
        if(!strcmp(input.c_str(), "confirm")){
            cout << "Send confirm.." << endl;
            QCL->confirm();
        }
        else if(!strncmp(input.c_str(), "start", 5)){
            cout << "getting start.." << endl;
            int n = atoi(input.c_str() + 5);
            QCL->start(n);
        }
    }
    eventManagerThread.stop();
}
