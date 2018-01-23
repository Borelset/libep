aux_source_directory(../ ROOTSRC)
aux_source_directory(../ep EPSRC)

aux_source_directory(../NetModule NETSRC)
aux_source_directory(../Utils UTILSSRC)
aux_source_directory(../Utils/Logger LOGSRC)
include_directories(../ ../ep ../NetModule ../Utils ../Utils/Logger)

add_executable(eptest main.cpp ${ROOTSRC} ${EPSRC} ${NETSRC} ${UTILSSRC} ${LOGSRC})
target_link_libraries(eptest pthread)