//
// Created by borelset on 1/15/18.
//

#include "TCPServer.h"

NetModule::TCPServer::TCPServer(int port):
        mAcceptor(std::make_shared(mEventManager), port)
{

}

NetModule::TCPServer::~TCPServer() {

}

void NetModule::TCPServer::start() {
    mAcceptor.listen();

}
