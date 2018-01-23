//
// Created by borelset on 1/12/18.
//

#ifndef LIBEP_UTILS_CURRENTTHREAD_H
#define LIBEP_UTILS_CURRENTTHREAD_H

#include <sched.h>
#include "Thread.h"
#include "Utils.h"

namespace Utils{
    namespace CurrentThread{
        static __thread pid_t threadPid = -1;

        inline pid_t gettid(){
            if(__builtin_expect(threadPid==-1, 0)){
                threadPid = Utils::getTid();
            }else{
                return threadPid;
            }
        }

        static void* callbackProxy(void* th){
            Thread* thread = (Thread*)th;
            thread->setTid(gettid());
            thread->getCallback()();
            return nullptr;
        }
    }
}


#endif //LIBEP_UTILS_CURRENTTHREAD_H
