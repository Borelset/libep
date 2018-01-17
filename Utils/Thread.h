//
// Created by borelset on 1/12/18.
//

#ifndef LIBEP_UTILS_THREAD_H
#define LIBEP_UTILS_THREAD_H

#include <zconf.h>
#include <functional>
#include "../ep/epollUtils.h"
#include "noncopyable.h"

namespace Utils{
    class Thread : Utils::noncopyable{
    public:
        typedef std::function<void()> Callback;
        explicit Thread(const Callback& callback);
        ~Thread();
        void run();
        int join();
        bool isRunning();
        Callback getCallback();
        void setTid(pid_t tid);
        pid_t getTid();
    private:
        pthread_t mPid;
        bool mIsRunning;
        Callback mCallback;
        pid_t mTid;
    };

}


#endif //LIBEP_UTILS_THREAD_H
