cmake_minimum_required(VERSION 3.10)

project(PooolSim)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O2")

add_subdirectory(libpoolsim)
add_subdirectory(poolsim)
if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()

