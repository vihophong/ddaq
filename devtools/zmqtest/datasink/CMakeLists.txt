# From https://root.cern.ch/faq/can-i-integrate-root-my-cmake-build

# CMakeLists.txt for event package. It creates a library with dictionary and a main program
cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(main)

# You need to tell CMake where to find the ROOT installation. This can be done in a number of ways:
#   - ROOT built with classic configure/make use the provided $ROOTSYS/etc/cmake/FindROOT.cmake
#   - ROOT built with CMake. Add in CMAKE_PREFIX_PATH the installation prefix for ROOT
list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})

#---Locate the ROOT package and defines a number of variables (e.g. ROOT_INCLUDE_DIRS)
#find_package(ROOT REQUIRED COMPONENTS Core RIO RooFit RooFitCore RooStats Hist Tree Matrix Physics MathCore Net)
find_package(ROOT REQUIRED COMPONENTS Core RIO Hist Tree Matrix Physics MathCore Net)

#---Locate cppzmq package
find_package(cppzmq)

#---Define useful ROOT functions and macros (e.g. ROOT_GENERATE_DICTIONARY)
include(${ROOT_USE_FILE})

include_directories(${CMAKE_SOURCE_DIR} ${ROOT_INCLUDE_DIRS})
add_definitions(${ROOT_CXX_FLAGS})


#---Create  a main program using the library
add_executable(online
    online.cc)
target_link_libraries(online ${ROOT_LIBRARIES})
target_link_libraries(online cppzmq)

add_executable(datasink
    datasink.cc)
target_link_libraries(datasink ${ROOT_LIBRARIES})
target_link_libraries(datasink cppzmq)

add_executable(daqread
    daqread.cc)
target_link_libraries(daqread ${ROOT_LIBRARIES})
target_link_libraries(daqread cppzmq)

