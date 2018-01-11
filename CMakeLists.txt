cmake_minimum_required(VERSION 3.9)
project(NetModel)

set(CMAKE_CXX_STANDARD 11)

add_executable(NetModel main.cpp Channel.h Channel.cpp Utils.h EventManager.h EpollHandler.h EpollHandler.cpp EventManager.cpp Timer.h Timer.cpp TimerQueue.h TimerQueue.cpp Mutex.h Mutex.cpp)

target_link_libraries(NetModel pthread)