//
// Created by borelset on 1/12/18.
//

#ifndef NETMODEL_THREAD_H
#define NETMODEL_THREAD_H

#include <zconf.h>
#include <functional>

namespace ep{
    class Thread{
    public:
        typedef std::function<void()> Callback;
        Thread(const Callback& callback);
        ~Thread();
        void run();
        int join();
        bool isRunning();
        Callback getCallback();
        void setTid(pid_t tid);
    private:
        pthread_t mPid;
        bool mIsRunning;
        Callback mCallback;
        pid_t mTid;
    };

}


#endif //NETMODEL_THREAD_H
