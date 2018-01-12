//
// Created by borelset on 1/12/18.
//

#ifndef NETMODEL_CURRENTTHREAD_H
#define NETMODEL_CURRENTTHREAD_H

#include <sched.h>
#include "Utils.h"
#include "Thread.h"
namespace ep{
    namespace CurrentThread{
        static __thread pid_t threadPid = -1;

        inline pid_t gettid(){
            if(__builtin_expect(threadPid==-1, 0)){
                threadPid = getTid();
            }else{
                return threadPid;
            }
        }

        static void* callbackProxy(void* th){
            std::cout << "Current::callbackProxy==>" << std::endl;
            ep::Thread* thread = (ep::Thread*)th;
            thread->setTid(gettid());
            thread->getCallback()();
            return nullptr;
        }
    }
}


#endif //NETMODEL_CURRENTTHREAD_H
