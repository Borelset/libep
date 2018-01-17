//
// Created by borelset on 1/16/18.
//

#ifndef LIBEP_NETUTILS_H
#define LIBEP_NETUTILS_H

#include <netinet/in.h>
#include <zconf.h>
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
}

#endif //LIBEP_NETUTILS_H
