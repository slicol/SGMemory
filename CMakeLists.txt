cmake_minimum_required(VERSION 3.2)
set(CMAKE_BUILD_TYPE "Release")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_OSX_DEPLOYMENT_TARGET "10.15" CACHE STRING "Minimum OS X deployment version")
#set(CMAKE_OSX_SYSROOT /Library/Developer/CommandLineTools/SDKs/MacOSX12.sdk)
# Make PROJECT_SOURCE_DIR, PROJECT_BINARY_DIR, and PROJECT_NAME available.

set(PROJECT_NAME SGMemory LANGUAGES CXX)
set (CMAKE_CXX_STANDARD 17)
project(${PROJECT_NAME})
message("=================================================")
message("================ SGMemory-Project ===============")
message("=================================================")
message(${PROJECT_SOURCE_DIR})
set(SGMemoryProjectDir ${PROJECT_SOURCE_DIR})


set(DIR_DEBUG_OUT ${PROJECT_BINARY_DIR}/lib/Debug)
set(DIR_RELEASE_OUT ${PROJECT_BINARY_DIR}/lib/Release)

if(MSVC)
    if (${CMAKE_BUILD_TYPE} STREQUAL "Release")
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MT")
    else()

    endif ()
add_definitions(-D MT_WIN_EV)
else()
add_definitions(-fdeclspec)
endif()

add_definitions(-DUNICODE -D_UNICODE)
add_definitions(-D WITH_EDITOR)


################################
# Normal Libraries & Executables
################################
add_subdirectory(SGMemory)
add_subdirectory(SGMemoryTest)


################################
# Install files
################################
