message("==================SGMemoryTest===================")

set(TARGET_NAME SGMemoryTest)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

file(GLOB_RECURSE HEADERS "*.h*")
file(GLOB_RECURSE SOURCES "*.cpp")


source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${HEADERS} ${SOURCES} ${TEMPLATES})

link_libraries(SGMemory)
include_directories(./)
include_directories(./../SGMemory/)

set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
set(CMAKE_INSTALL_RPATH "@loader_path")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# header files are superflous, but some IDEs (Visual Studio) don't include
# them in the solution explorer without them being added to the list of sources
add_executable(${TARGET_NAME} ${HEADERS} ${SOURCES} ${TEMPLATES})
set_target_properties(${TARGET_NAME} PROPERTIES CXX_STANDARD 17)
