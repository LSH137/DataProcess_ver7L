cmake_minimum_required(VERSION 3.15)
project(DataProcess_ver7L)

set(CMAKE_CXX_STANDARD 14)

add_executable(DataProcess_ver7L main.cpp source/FileIO.cpp source/FileIO.h source/Process.cpp source/Process.h source/Constant.cpp source/Constant.h source/Gathering.cpp source/Gathering.h source/Class.cpp source/Class.h source/MakeDir.cpp source/MakeDir.h source/CreateFile.cpp source/CreateFile.h source/BuildG4Proj.cpp source/BuildG4Proj.h source/AutoRun.cpp source/AutoRun.h source/Graph&Histogram.cpp source/Graph&Histogram.h)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "/home/me/ClionProjects/DataProcess_ver6L")
set(CMAKE_GENERATOR_PLATFORM=x64)
#set(FILE_OFFSET_BITS=64)
#SET ( CMAKE_C_FLAGS_RELEASE "-DCONFIG_RELEASE -O2" )