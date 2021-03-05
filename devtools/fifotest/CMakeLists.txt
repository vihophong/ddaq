cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(devtools)

include_directories(${CMAKE_SOURCE_DIR})

set(MAKEFIFOS_SOURCE makefifos.cpp)

set(VIEWFIFOS_SOURCE viewfifos.cpp)

add_executable(makefifos ${MAKEFIFOS_SOURCE})

add_executable(viewfifos ${VIEWFIFOS_SOURCE})

