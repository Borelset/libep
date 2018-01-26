//
// Created by borelset on 1/15/18.
//

#ifndef LIBEP__NETMODULE_SOCKET_H
#define LIBEP__NETMODULE_SOCKET_H

#include <netinet/in.h>
#include "../Utils/noncopyable.h"
#include "../Utils/FD.h"
#include "SockAddr.h"

namespace NetModule{
    constexpr static int MAX_ACCEPT_CONNECTION = 10;

    class Socket : Utils::noncopyable{
    public:
        explicit Socket(int domain = AF_INET,
               int type = SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC,
               int protocol = IPPROTO_TCP);
        explicit Socket(Utils::FD& fd, SockAddr addr);
        ~Socket();
        void bindAddr(const char* ip, int port);
        int connect(SockAddr addr);
        void listen();
        int accept(struct sockaddr_in*);
        int getSocket();
        void shutDown();
        void setKeepAlive(bool opt);
    private:
        SockAddr mAddr;
        Utils::FD mSocketFd;
        int mDomain;
        int mType;
        int mProtocol;
    };
}

#endif //LIBEP__NETMODULE_SOCKET_H
