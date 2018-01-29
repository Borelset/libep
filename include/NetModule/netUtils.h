//
// Created by borelset on 1/16/18.
//

#ifndef LIBEP_NETUTILS_H
#define LIBEP_NETUTILS_H

#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>

namespace NetModule{

    static sockaddr_in getLocalAddr(int fd){
        struct sockaddr_in localaddr;
        bzero(&localaddr, sizeof localaddr);
        socklen_t socklen = sizeof localaddr;
        getsockname(fd, (struct sockaddr*)&localaddr, &socklen);
        return localaddr;
    }

    static sockaddr_in getPeerAddr(int fd){
        struct sockaddr_in peeraddr;
        bzero(&peeraddr, sizeof peeraddr);
        socklen_t  socklen = sizeof peeraddr;
        getpeername(fd, (struct sockaddr*)&peeraddr, &socklen);
        return peeraddr;
    }

    static bool isSelfConnect(int fd){
        struct sockaddr_in localAddr = getLocalAddr(fd);
        struct sockaddr_in peerAddr = getPeerAddr(fd);
        if(
            localAddr.sin_port == peerAddr.sin_port &&
            localAddr.sin_addr.s_addr == peerAddr.sin_addr.s_addr
                ){
            return true;
        }else{
            return false;
        }
    }

    static int getSockError(int fd){
        int opt = 0;
        socklen_t optlen = sizeof opt;
        if(getsockopt(fd, SOL_SOCKET, SO_ERROR, &opt, &optlen) < 0){
            return errno;
        }else{
            return opt;
        }
    }
}

#endif //LIBEP_NETUTILS_H
