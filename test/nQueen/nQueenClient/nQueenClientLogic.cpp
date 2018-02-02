//
// Created by borelset on 2/1/18.
//

#include "nQueenClientLogic.h"
#include <string.h>
#include <sys/time.h>

nQueenClientLogic::ClientState nQueenClientLogic::stateParser(string &str) {
    const char* strPtr = str.c_str();
    if(!strncmp(strPtr,
                ClientStateString[(int)ClientState::Ready],
                strlen(ClientStateString[(int)ClientState::Ready]))){
        str = str.c_str() + strlen(ClientStateString[(int)ClientState::Ready]);
        return ClientState::Ready;

    }else if(!strncmp(strPtr,
                      ClientStateString[(int)ClientState::Result],
                      strlen(ClientStateString[(int)ClientState::Result]))){
        str = str.c_str() + strlen(ClientStateString[(int)ClientState::Result]);
        return ClientState::Result;

    }else{
        return ClientState::Unknown;
    }
}

void nQueenClientLogic::parseResult(string str, int &total, int &index, int &result) {
    int temp = 0;
    for(int i=0; i<str.size(); i++){
        if(str.c_str()[i] <= '9' && str.c_str()[i] >= '0'){
            temp *= 10;
            temp += str.c_str()[i] - '0';
        }else if(str.c_str()[i] == '@'){
            index = temp;
            temp = 0;
        }else if(str.c_str()[i] == '='){
            total = temp;
            temp = 0;
        }
    }
    result = temp;
}

nQueenClientLogic::nQueenClientLogic(ep::EventManager* eventManager):
        mEventManager(eventManager),
        mActiveServer(0),
        mProcessCondition(mProcessLock)
{

}

string nQueenClientLogic::readyCallback(string &content, const string &connName) {
    string result;
    //while(!mStart){
    //    Utils::MutexLockGuard lockGuard(mStartLock);
    //    mStartCondition.wait();
    //}
    int n = mLeftTasks.pop_back();
    mConfirmedTasks.insert(connName, n);

    char buffer[1024];
    sprintf(buffer, "Task %d@%d", n, mN);
    result = buffer;
    return result;
}

string nQueenClientLogic::resultCallback(string &content, const string &connName) {
    string result;
    int n = -1, r = -1, t = -1;
    parseResult(content, t, n, r);
    if(mN == t && mConfirmedTasks.get(connName) == n){
        mConfirmedTasks.erase(content);
        {
            Utils::MutexLockGuard lockGuard(mResultLock);
            mResult += r;
        }
    }
    mConfirmedTasks.erase(connName);
    cout << "Task: " << content << endl;
    if(mLeftTasks.size()!=0){
        result = readyCallback(content, connName);
    }
    if(mLeftTasks.size() == 0 && mConfirmedTasks.size() == 0){
        mT2 = getTime();
        cout << "result:" << mResult << endl;
        cout << "duration:" << (mT2 - mT1) << "ms" << endl;
        mProcessCondition.notify();
    }
    return result;
}

string nQueenClientLogic::errorCallback(string &content, const string &connName) {
    return std::__cxx11::string();
}

void nQueenClientLogic::start(int n) {
    mN = n;
    mResult = 0;
    for(int i=0; i<n; i++){
        mLeftTasks.push_back(i);
    }
    mConfirmedTasks.clear();
    mT1 = getTime();

    string confirmStr = "Confirm";
    for(int i=0; i<mActiveServer; i++){
        mClients[i]->send(confirmStr);
    }
    {
        Utils::MutexLockGuard lockGuard(mProcessLock);
        mProcessCondition.wait();
    }
}

void nQueenClientLogic::setClient(NetModule::SockAddr & addr) {
    if(mActiveServer >= MAX_SERVER_CONNECTION) return;
    char buffer[1024];
    sprintf(buffer, "Client#%d", mActiveServer);
    mClients[mActiveServer].reset(
            new NetModule::TCPClient(
                    mEventManager,
                    addr,
                    buffer)
    );
    mClients[mActiveServer]->setMessageCallback(mMessageCallback);

    mClients[mActiveServer]->connect();
    mActiveServer++;
}

void nQueenClientLogic::setMessageCallback(const nQueenClientLogic::MessageCallback & callback) {
    mMessageCallback = callback;
}

int64_t nQueenClientLogic::getTime() {
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    int64_t time = tv.tv_sec*1000 + tv.tv_usec/1000;
    return time;
}
