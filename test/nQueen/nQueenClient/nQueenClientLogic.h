//
// Created by borelset on 2/1/18.
//

#ifndef EP_NQUEENCLIENTLOGIC_H
#define EP_NQUEENCLIENTLOGIC_H


#include <string>
#include <functional>
#include "Utils/Mutex.h"
#include "test/nQueen/ThreadSafeStruct.h"
#include "NetModule/TCPClient.h"

using namespace std;

class nQueenClientLogic{
public:
    constexpr static int MAX_SERVER_CONNECTION = 10;

    enum class ClientState{
        Ready,
        Result,
        Unknown
    };
    constexpr static char* ClientStateString[] = {
            "Ready",
            "Result"
    };
    typedef function<void()> Callback;
    typedef function<void(weak_ptr<NetModule::TCPConnection> tcpc, time_t time)> MessageCallback;
    static ClientState stateParser(string &str);
    explicit nQueenClientLogic(ep::EventManager*);
    string readyCallback(string &content, const string &connName);
    string resultCallback(string &content, const string &connName);
    string errorCallback(string &content, const string &connName);
    void start(int n);
    void setClient(NetModule::SockAddr&);
    void setMessageCallback(const MessageCallback&);
private:
    ep::EventManager* mEventManager;
    unique_ptr<NetModule::TCPClient> mClients[MAX_SERVER_CONNECTION];
    int mN;
    int mActiveServer;
    TSVector<int> mLeftTasks;
    TSMap<string, int> mConfirmedTasks;
    Utils::MutexLock mResultLock;
    Utils::MutexLock mProcessLock;
    int mResult;
    Utils::Condition mProcessCondition;
    int64_t mT1, mT2;
    MessageCallback mMessageCallback;

    int64_t getTime();
    static void parseResult(string str, int &total, int &index, int &result);
};


#endif //EP_NQUEENCLIENTLOGIC_H
