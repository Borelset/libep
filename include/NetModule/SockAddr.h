//
// Created by borelset on 1/15/18.
//

#ifndef LIBEP_NETMODULE_SOCKADDR_H
#define LIBEP_NETMODULE_SOCKADDR_H

#include <netinet/in.h>
#include <string>

namespace NetModule{
    class SockAddr{
    public:
        SockAddr();
        explicit SockAddr(struct sockaddr_in);
        SockAddr(const char* ip, int port);
        void setAddr(const char* ip, int port);
        struct sockaddr_in &getAddr();
    private:
        sockaddr_in mRawSockAddr;
    };
}


#endif //LIBEP_NETMODULE_SOCKADDR_H
