//
// Created by borelset on 1/18/18.
//

#ifndef LIBEP_LOGGERUTILS_H
#define LIBEP_LOGGERUTILS_H

#include "LoggerManager.h"

namespace Log{
    static void SetLogPath(char* path){
        gLoggerManager.setLogPath(path);
    }

    static void SetLogLevel(LogLevel level){
        gLoggerManager.setLogLevel(level);
    }
}

#endif //LIBEP_LOGGERUTILS_H
