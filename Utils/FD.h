//
// Created by borelset on 1/15/18.
//

#ifndef LIBEP_UTILS_FD_H
#define LIBEP_UTILS_FD_H

#include "noncopyable.h"

namespace Utils{
    class FD : noncopyable{
    public:
        FD(int fd);
        ~FD();
        int getFd();
    private:
        int mFd;
    };
}

#endif //LIBEP_UTILS_FD_H
