//
// Created by borelset on 2/1/18.
//

#ifndef EP_NQUEENSERVERLOGIC_H
#define EP_NQUEENSERVERLOGIC_H


#include <string>
#include <functional>
#include <vector>
#include <map>
#include "Utils/Mutex.h"
#include "test/nQueen/ThreadSafeStruct.h"

using namespace std;

class nQueenServerLogic{
public:
    enum class ServerState{
        Confirm,
        Task,
        Unknown
    };
    constexpr static char* ServerStateString[] = {
            "Confirm",
            "Task"
    };
    typedef function<void()> Callback;
    static ServerState stateParser(string &str);
    static void parseTask(string str, int &total, int &index);
    static string confirmCallback(string &content, const string &connName);
    static string taskCallback(string &content, const string &connName);
    static string errorCallback(string &content, const string &connName);
};


#endif //EP_NQUEENSERVERLOGIC_H
