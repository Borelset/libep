//
// Created by borelset on 2/1/18.
//

#ifndef EP_THREADSAFESTRUCT_H
#define EP_THREADSAFESTRUCT_H

#include <vector>
#include <map>
#include "Utils/Mutex.h"

template <typename T>
class TSVector{
public:
    void push_back(const T& item){
        Utils::MutexLockGuard localLock(mLock);
        mVector.push_back(item);
    }
    T pop_back(){
        Utils::MutexLockGuard localLock(mLock);
        T result = mVector.back();
        mVector.pop_back();
        return result;
    }
    int size(){
        Utils::MutexLockGuard localLock(mLock);
        return mVector.size();
    }
private:
    std::vector<T> mVector;
    Utils::MutexLock mLock;
};

template <typename K, typename T>
class TSMap{
public:
    bool find(const K& key){
        auto iter = mMap.find(key);
        if(iter == mMap.end()){
            return false;
        }else{
            return true;
        }
    }
    bool erase(const K& key){
        auto iter = mMap.find(key);
        if(iter != mMap.end()){
            Utils::MutexLockGuard localLock(mLock);
            mMap.erase(iter);
            return true;
        }
        return false;
    }
    void insert(const K& key, const T& value){
        Utils::MutexLockGuard localLock(mLock);
        mMap[key] = value;
    }
    T& get(const K& key){
        Utils::MutexLockGuard localLock(mLock);
        return mMap[key];
    }
    int size(){
        return mMap.size();
    }

private:
    std::map<K, T> mMap;
    Utils::MutexLock mLock;
};

#endif //EP_THREADSAFESTRUCT_H
