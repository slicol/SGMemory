cmake_minimum_required(VERSION 3.2)

message("==================SGMemory===================")

project(SGMemory)
message(${PROJECT_BINARY_DIR})
message(${PROJECT_SOURCE_DIR})
set_property(GLOBAL PROPERTY USE_FOLDERS ON)


message("current source dir ${CMAKE_CURRENT_SOURCE_DIR}")
#=================Prebuild script=======================

#=======================================================
include_directories(./)

file(GLOB_RECURSE HEADERS "*.h*")
file(GLOB_RECURSE SOURCES "*.cpp" "*.c")

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${HEADERS} ${SOURCES})

file(GLOB Project_All ./*.*)
list(APPEND AllModuleFiles ${Project_HPP} ${Project_CPP} ${HEADERS} ${SOURCES})


if(MSVC)
    if (${CMAKE_BUILD_TYPE} STREQUAL "Release")
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MT")
    else()

    endif ()
endif()

add_library(SGMemory STATIC ${AllModuleFiles} ${Project_All})
target_compile_features(SGMemory PRIVATE cxx_std_17)


##VS2017设置默认管理员权限启动
##找到vs安装目录下的：C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\Common7\IDE\devenv.exe
##右键->兼容性疑难解答->疑难解答程序->勾选【该程序需要附加权限】->测试后下一步->保存设置。

