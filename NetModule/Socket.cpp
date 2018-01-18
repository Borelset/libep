//
// Created by borelset on 1/15/18.
//

#include <sys/socket.h>
#include <string.h>
#include <iostream>
#include "Socket.h"
#include "arpa/inet.h"

NetModule::Socket::Socket(int domain, int type, int protocol):
        mSocketFd(socket(domain, type, protocol)),
        mDomain(domain),
        mType(type),
        mProtocol(protocol)
{
    if(mSocketFd.getFd() < 0){
        std::cout << "NetModule::Socket::Socket==>"
                  << "bad socket fd" << std::endl;
        exit(0);
    }
    bzero(&mAddr, sizeof mAddr);
}

void NetModule::Socket::bindAddr(const char* ip, int port) {
    mAddr.setAddr(ip, port);
    if(bind(mSocketFd.getFd(), (sockaddr*)&mAddr.getAddr(), sizeof mAddr.getAddr()) < 0){
        std::cout << "NetModule::Socket::bind==>"
                  << "fail in bind" << std::endl;
        exit(0);
    }
}

int NetModule::Socket::getSocket() {
    return mSocketFd.getFd();
}

void NetModule::Socket::listen() {
    if(::listen(mSocketFd.getFd(), MAX_ACCEPT_CONNECTION) < 0){
        std::cout << "NetModule::Socket::listen==>>"
                  << "fail in listen" << std::endl;
    }
}

int NetModule::Socket::accept(struct sockaddr_in * saddr) {
    socklen_t sockaddrlen = sizeof *saddr;
    int fd = ::accept(mSocketFd.getFd(), (sockaddr*)saddr, &sockaddrlen);
    if(fd < 0){
        std::cout << "NetModule::Socket::accept==>"
                  << "fail in accept connection" << std::endl;
    }
    return fd;
}

NetModule::Socket::~Socket() {
    std::cout << "NetModule::Socket::~Socket"
              << "Socket with fd:" << mSocketFd.getFd() << " from " << inet_ntoa(mAddr.getAddr().sin_addr) << " releasing.." << std::endl;
}

NetModule::Socket::Socket(Utils::FD& fd, SockAddr addr):
        mSocketFd(fd.getFd()),
        mAddr(addr)
{

}

void NetModule::Socket::shutDown() {
    shutdown(mSocketFd.getFd(), SHUT_WR);
}

void NetModule::Socket::setKeepAlive(bool opt) {
    int option = opt? 1 : 0;
    setsockopt(mSocketFd.getFd(), SOL_SOCKET, SO_KEEPALIVE, &option, sizeof option);
}

bool NetModule::Socket::connect(NetModule::SockAddr addr) {
    if(::connect(mSocketFd.getFd(), (sockaddr*)&addr.getAddr(), sizeof (struct sockaddr)) < 0){
        std::cout << "NetModule::Socket::connect==>"
                  << "Connect error" << std::endl;
        return false;
    }
    return true;
}

