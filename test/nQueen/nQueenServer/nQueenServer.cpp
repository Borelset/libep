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

void messageCallback(weak_ptr<TCPConnection> tcpc, time_t time){
    shared_ptr<TCPConnection> sharedPtr = tcpc.lock();
    string received = sharedPtr->getReadBuffer()->getContent();
    cout << "receive:" << received << endl;
    string message;
    nQueenServerLogic::ServerState serverState =
            nQueenServerLogic::stateParser(received);
    switch (serverState){
        case nQueenServerLogic::ServerState::Confirm:
            message = nQueenServerLogic::confirmCallback(
                    received, sharedPtr->getName()
            );
            break;
        case nQueenServerLogic::ServerState::Task:
            message = nQueenServerLogic::taskCallback(
                    received,
                    sharedPtr->getName()
            );
            break;
        case nQueenServerLogic::ServerState::Unknown:
        default:
            message = nQueenServerLogic::errorCallback(
                    received,
                    sharedPtr->getName()
            );
    }

    cout << "send: " << message <<endl;
    if(message.size()>0){
        sharedPtr->send(message);
    }
}

TCPServer* serverPtr;

int main(int argc, char** argv){
    if(argc != 2){
        cout << "wrong args.." << endl;
        return 0;
    }
    int port = atoi(argv[1]);
    TCPServer mServer(port);
    serverPtr = &mServer;
    mServer.setTimingWheelCircle(0);
    mServer.setMessageCallback(messageCallback);
    mServer.start();
}

