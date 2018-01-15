cmake_minimum_required(VERSION 3.9)
project(libep)

set(CMAKE_CXX_STANDARD 11)

add_executable(libep main.cpp ep/Channel.h ep/Channel.cpp ep/epollUtils.h ep/EventManager.h ep/EpollHandler.h ep/EpollHandler.cpp ep/EventManager.cpp ep/Timer.h ep/Timer.cpp ep/TimerQueue.h ep/TimerQueue.cpp Utils/Mutex.h Utils/Mutex.cpp Utils/CurrentThread.h Utils/Thread.h Utils/Thread.cpp ep/EventManagerThread.h ep/EventManagerThread.cpp Utils/FD.h Utils/FD.cpp NetModule/Acceptor.h Utils/noncopyable.h Utils/Utils.h NetModule/Socket.h NetModule/Socket.cpp NetModule/Acceptor.cpp NetModule/SockAddr.h NetModule/SockAddr.cpp NetModule/TCPServer.h NetModule/TCPConnection.h NetModule/TCPServer.cpp)

target_link_libraries(libep pthread)
