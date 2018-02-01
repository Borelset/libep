//
// Created by borelset on 2/1/18.
//

#include "nQueenServerLogic.h"
#include <cstring>

nQueenServerLogic::ServerState nQueenServerLogic::statePrasor(string &str) {
    const char* strPtr = str.c_str();
    if(!strncmp(strPtr,
                ServerStateString[(int)ServerState::Ready],
                strlen(ServerStateString[(int)ServerState::Ready]))){
        str = str.c_str() + strlen(ServerStateString[(int)ServerState::Ready]);
        return ServerState::Ready;

    }else if(!strncmp(strPtr,
                      ServerStateString[(int)ServerState::Task],
                      strlen(ServerStateString[(int)ServerState::Task]))){
        str = str.c_str() + strlen(ServerStateString[(int)ServerState::Task]);
        return ServerState::Task;

    }else if(!strncmp(strPtr,
                      ServerStateString[(int)ServerState::Result],
                      strlen(ServerStateString[(int)ServerState::Result]))){
        str = str.c_str() + strlen(ServerStateString[(int)ServerState::Result]);
        return ServerState::Result;

    }else{
        return ServerState::Unknown;
    }
}

void praseResult(string str, int& total, int& index, int& result){
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

void praseTask(string str, int& total, int& index){
    int temp = 0;
    for(int i=0; i<str.size(); i++){
        if(str.c_str()[i] <= '9' && str.c_str()[i] >= '0'){
            temp *= 10;
            temp += str.c_str()[i] - '0';
        }else if(str.c_str()[i] == '@'){
            index = temp;
            temp = 0;
        }
    }
    total = temp;
}

string nQueenServerLogic::readyCallback(string &content, const string &connName) {
    string result;
    while(!mStart){
        mCondition.wait();
    }
    if(!mType) return result;
    if(mConfirmedTasks.find(connName)){
        int n = mConfirmedTasks.get(connName);
        mLeftTasks.push_back(n);
        mConfirmedTasks.erase(connName);
        return result;
    }

    int n = -1;
    n = mLeftTasks.pop_back();
    char buffer[1024];
    sprintf(buffer, "Task %d@%d", n, mN);

    result = buffer;
    return result;
}

string nQueenServerLogic::taskCallback(string &content, const string &connName) {
    // solve
    cout << "taskCallback" << endl;
    int n = -1, t = -1;
    praseTask(content, t, n);
    mN = t;
    char buffer[1024];
    sprintf(buffer, "Result %d@%d=%d", n, mN, 1);
    string result = buffer;

    return result;
}

string nQueenServerLogic::resultCallback(string &content, const string &connName) {
    string result;
    if(!mType) return result;
    int n = -1, r = -1, t = -1;
    praseResult(content, t, n, r);
    if(mN == t && mConfirmedTasks.get(connName) == n){
        mConfirmedTasks.erase(content);
        {
            Utils::MutexLockGuard lockGuard(mutexLock);
            mResult += r;
        }
    }
    mConfirmedTasks.erase(connName);
    cout << "Task: " << content << endl;
    cout << "Left:" << mLeftTasks.size() << " Comfirmed:" << mConfirmedTasks.size() << endl;
    if(mLeftTasks.size()!=0){
        result = readyCallback(content, connName);
    }
    if(mLeftTasks.size() == 0 && mConfirmedTasks.size() == 0){
        mT2 = clock();
        cout << "result:" << mResult << endl;
        cout << "duration:" << mT2 - mT1 << endl;
    }
    return result;
}

string nQueenServerLogic::errorCallback(string &content, const string &connName) {

}

nQueenServerLogic::nQueenServerLogic(int n):
        mN(n),
        mType(n>=0?true:false),
        mResult(0),
        mCondition(mStartLock),
        mStart(false)
{
    for(int i=0; i<n; i++){
        mLeftTasks.push_back(i);
    }
}

void nQueenServerLogic::solver() {

}

int nQueenServerLogic::getResult() {
    return mResult;
}

void nQueenServerLogic::start() {
    mT1 = clock();
    mCondition.notifyAll();
    mStart = true;
}
