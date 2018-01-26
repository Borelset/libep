cmake_minimum_required(VERSION 3.9)
project(ep)

set(CMAKE_CXX_STANDARD 11)

aux_source_directory(./ ROOTSRC)
aux_source_directory(./ep EPSRC)
aux_source_directory(./NetModule NETSRC)
aux_source_directory(./Utils UTILSSRC)
aux_source_directory(./Utils/Logger LOGSRC)
aux_source_directory(./test TESTSRC)
include_directories(./ ./ep ./NetModule ./Utils ./Utils/Logger)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/build/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/build/lib)

add_library(ep SHARED ${ROOTSRC} ${EPSRC} ${NETSRC} ${UTILSSRC} ${LOGSRC})
target_link_libraries(ep pthread)

add_executable(eptest ${ROOTSRC} ${EPSRC} ${NETSRC} ${UTILSSRC} ${LOGSRC} ${TESTSRC})
target_link_libraries(eptest pthread)

install(TARGETS ep
        LIBRARY DESTINATION lib)
