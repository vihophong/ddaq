# From https://root.cern.ch/faq/can-i-integrate-root-my-cmake-build
#macro(print_all_variables)
#    message(STATUS "print_all_variables------------------------------------------{")
#    get_cmake_property(_variableNames VARIABLES)
#    foreach (_variableName ${_variableNames})
#        message(STATUS "${_variableName}=${${_variableName}}")
#    endforeach()
#    message(STATUS "print_all_variables------------------------------------------}")
#endmacro()

# CMakeLists.txt for event package. It creates a library with dictionary and a main program
cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(caendppphaplugin)

# You need to tell CMake where to find the ROOT installation. This can be done in a number of ways:
#   - ROOT built with classic configure/make use the provided $ROOTSYS/etc/cmake/FindROOT.cmake
#   - ROOT built with CMake. Add in CMAKE_PREFIX_PATH the installation prefix for ROOT


#---Define useful ROOT functions and macros (e.g. ROOT_GENERATE_DICTIONARY)

include_directories(${CMAKE_SOURCE_DIR} $ENV{ONLINE_MAIN}/include)

link_directories(/usr/lib $ENV{ONLINE_MAIN}/lib)

#set(BUILD_CXX_FLAGS2 "-lz")
#add_definitions(${BUILD_CXX_FLAGS2})

add_definitions(-DLINUX=1)

#print_all_variables()

#include source and include dir
include_directories(${PROJECT_SOURCE_DIR}/include)

file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.h)

add_library(rcdaqplugin_CAENdrsPADC SHARED ${headers} ${sources})
target_link_libraries(rcdaqplugin_CAENdrsPADC CAENPeakSensing)

#-- Excutable tools

add_executable(caenpadcconfig padcconfig.cc)
target_link_libraries(caenpadcconfig CAENPeakSensing)

#----------------------------------------------------------------------------
# Copy all resources including scripts, examples, text files, etc... to the build directory, i.e. the directory in which we
# build project. This is so that we can us the resources because it
# relies on these scripts being in the current working directory.
#
set(PROJ_DATA
  resource1.txt
  resource2.txt
  )


foreach(_script ${PROJ_DATA})
  configure_file(
    ${PROJECT_SOURCE_DIR}/${_script}
    ${PROJECT_BINARY_DIR}/${_script}
    COPYONLY
    )
endforeach()

#----------------------------------------------------------------------------
# Install the binary/library/include to 'bin/lib/includes' directory under CMAKE_INSTALL_PREFIX
#

#pmonitor
install(TARGETS rcdaqplugin_CAENdrsPADC
        LIBRARY DESTINATION lib
)

#binary installation
install(TARGETS caenpadcconfig DESTINATION bin)


