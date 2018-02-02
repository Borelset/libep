//
// Created by borelset on 2/1/18.
//

#include "test/nQueen/nQueenClient/nQueenClientLogic.h"
#include <iostream>
#include <functional>
#include <NetModule/TCPConnection.h>
#include <NetModule/TCPServer.h>
#include <cstring>
#include "ServerAddressParser.h"
#include "Menu.h"

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

void menuInitCallback(){
    cout << "N Queen Solver Client" << endl;
}

void startCallback(vector<string> args){
    if(args.size() <= 1){
        cout << "Usage: start [n]" << endl;
        return;
    }
    int n = atoi(args[1].c_str());
    QCL->start(n);
}

int main(int argc, char** argv){
    if(argc == 1){
        cout << "No server lists provided\n";
        return 0;
    }

    vector<NetModule::SockAddr> serverList = SAParser(argv[1]);

    ep::EventManagerThread eventManagerThread;
    eventManagerThread.start();
    ep::EventManager* eventManager = eventManagerThread.getEventManagerHandler();
    if(eventManager == nullptr){
        cout << "Something wrong..\nFail to run a thread" << endl;
    }
    nQueenClientLogic qcl(eventManager);
    qcl.setMessageCallback(messageCallback);
    QCL = &qcl;
    for(int i=0; i<serverList.size(); i++){
        qcl.setClient(serverList[i]);
    }

    Menu mMenu;
    mMenu.setInit(menuInitCallback);
    mMenu.addCommand(
            MenuItem(
                    "start",
                    "Get the amount of solutions to N Queen problem",
                    startCallback)
    );
    mMenu.startMenu();
    eventManagerThread.stop();
}
