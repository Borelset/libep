cmake_minimum_required(VERSION 3.9)
project(ep)

set(CMAKE_CXX_STANDARD 11)

aux_source_directory(./ ROOTSRC)
aux_source_directory(./ep EPSRC)
aux_source_directory(./NetModule NETSRC)
aux_source_directory(./Utils UTILSSRC)
aux_source_directory(./Utils/Logger LOGSRC)

include_directories(./ ./ep ./NetModule ./Utils ./Utils/Logger ./test/nQueen)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/build/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/build/lib)

add_library(ep SHARED ${ROOTSRC} ${EPSRC} ${NETSRC} ${UTILSSRC} ${LOGSRC})
target_link_libraries(ep pthread)

add_executable(TCPServerTest ${ROOTSRC} ${EPSRC} ${NETSRC} ${UTILSSRC} ${LOGSRC} ./test/TCPServerTest.cpp)
target_link_libraries(TCPServerTest pthread)

add_executable(TCPClientTest ${ROOTSRC} ${EPSRC} ${NETSRC} ${UTILSSRC} ${LOGSRC} ./test/TCPClientTest.cpp)
target_link_libraries(TCPClientTest pthread)

add_executable(nQueenServer ${ROOTSRC} ${EPSRC} ${NETSRC} ${UTILSSRC} ${LOGSRC} ./test/nQueen/MainServer.cpp ./test/nQueen/nQueenServerLogic.h ./test/nQueen/nQueenServerLogic.cpp ./test/nQueen/ThreadSafeStruct.h)
target_link_libraries(nQueenServer pthread)

add_executable(nQueenSlave ${ROOTSRC} ${EPSRC} ${NETSRC} ${UTILSSRC} ${LOGSRC} ./test/nQueen/SlaveServer.cpp ./test/nQueen/nQueenServerLogic.h ./test/nQueen/nQueenServerLogic.cpp ./test/nQueen/ThreadSafeStruct.h)
target_link_libraries(nQueenSlave pthread)


install(TARGETS ep
        LIBRARY DESTINATION lib)
