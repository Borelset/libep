//
// Created by borelset on 1/15/18.
//

#include <arpa/inet.h>
#include "SockAddr.h"

using namespace NetModule;

SockAddr::SockAddr(struct sockaddr_in raddr):
        mRawSockAddr(raddr)
{

}

SockAddr::SockAddr(const char *ip, int port) {
    mRawSockAddr.sin_port = htons(port);
    mRawSockAddr.sin_addr.s_addr = inet_addr(ip);
    mRawSockAddr.sin_family = AF_INET;
}

struct sockaddr_in &SockAddr::getAddr() {
    return mRawSockAddr;
}

void SockAddr::setAddr(const char* ip, int port) {
    mRawSockAddr.sin_port = htons(port);
    mRawSockAddr.sin_addr.s_addr = inet_addr(ip);
    mRawSockAddr.sin_family = AF_INET;
}

SockAddr::SockAddr() {

}
