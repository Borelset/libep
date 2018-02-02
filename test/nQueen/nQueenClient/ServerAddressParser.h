//
// Created by borelset on 2/2/18.
//

#ifndef EP_TEXTPARSER_H
#define EP_TEXTPARSER_H

#include "unistd.h"
#include "SockAddr.h"
#include "vector"

std::vector<NetModule::SockAddr> SAParser(const char* path);

#endif //EP_TEXTPARSER_H
