//
// Created by borelset on 2/1/18.
//

#ifndef EP_MAINSERVERLOGIC_H
#define EP_MAINSERVERLOGIC_H

#include <string>
#include <functional>
#include <vector>
#include <map>
#include "Utils/Mutex.h"
#include "ThreadSafeStruct.h"

using namespace std;

class nQueenServerLogic{
public:
    enum class ServerState{
        Ready,
        Task,
        Result,
        Unknown
    };
    constexpr static char* ServerStateString[] = {
            "Ready",
            "Task",
            "Result"
    };
    typedef function<void()> Callback;
    static ServerState statePrasor(string &str);
    explicit nQueenServerLogic(int = 0);
    string readyCallback(string &content, const string &connName);
    string taskCallback(string &content, const string &connName);
    string resultCallback(string &content, const string &connName);
    string errorCallback(string &content, const string &connName);
    static void solver();
    int getResult();
    void start();
private:
    int mN;
    bool mType;
    TSVector<int> mLeftTasks;
    TSMap<string, int> mConfirmedTasks;
    Utils::MutexLock mutexLock;
    Utils::MutexLock mStartLock;
    int mResult;
    Utils::Condition mCondition;
    bool mStart;
    clock_t mT1, mT2;
};


#endif //EP_MAINSERVERLOGIC_H
