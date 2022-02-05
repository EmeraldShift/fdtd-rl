# Needs to be changed appropriately
CMAKE_MINIMUM_REQUIRED (VERSION 3.12 FATAL_ERROR)

#SET(MULTI_TESTS 0 CACHE BOOL "If set, builds the multi-threaded tests.")
#SET(NATIVE_TESTS 0 CACHE BOOL "If set, builds the native input set tests.")

PROJECT(fdtd CXX)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W -Wall -Wextra -pedantic -std=gnu++17")

FIND_PACKAGE(PkgConfig REQUIRED)

PKG_CHECK_MODULES(RAFTLIB REQUIRED IMPORTED_TARGET raftlib)

ADD_EXECUTABLE(fdtd main.cpp fdtd.cpp kernels.cpp)
TARGET_LINK_LIBRARIES(fdtd PUBLIC PkgConfig::RAFTLIB)

#ENABLE_TESTING()
#ADD_SUBDIRECTORY(tests)
