cmake_minimum_required(VERSION 3.9)
project(NetModel)

set(CMAKE_CXX_STANDARD 11)

add_executable(NetModel main.cpp ep/Channel.h ep/Channel.cpp ep/Utils.h ep/EventManager.h ep/EpollHandler.h ep/EpollHandler.cpp ep/EventManager.cpp ep/Timer.h ep/Timer.cpp ep/TimerQueue.h ep/TimerQueue.cpp ep/Mutex.h ep/Mutex.cpp ep/CurrentThread.h ep/Thread.h ep/Thread.cpp ep/EventManagerThread.h ep/EventManagerThread.cpp)

target_link_libraries(NetModel pthread)