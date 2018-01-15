//
// Created by borelset on 1/15/18.
//

#ifndef LIBEP_UTILS_UTILS_H
#define LIBEP_UTILS_UTILS_H

#include <ctime>

namespace Utils{
    static time_t getTime(){
        time_t time_;
        time(&time_);
        return time_;
    }

    static pid_t getTid(){
        return syscall(SYS_gettid);
    }
}

#endif //LIBEP_UTILS_UTILS_H
