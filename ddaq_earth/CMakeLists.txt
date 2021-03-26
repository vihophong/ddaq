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
project(ddaq)

# You need to tell CMake where to find the ROOT installation. This can be done in a number of ways:
#   - ROOT built with classic configure/make use the provided $ROOTSYS/etc/cmake/FindROOT.cmake
#   - ROOT built with CMake. Add in CMAKE_PREFIX_PATH the installation prefix for ROOT
#list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})

#---Locate the ROOT package and defines a number of variables (e.g. ROOT_INCLUDE_DIRS)
#find_package(ROOT REQUIRED COMPONENTS Core RIO RooFit RooFitCore RooStats Hist Tree Matrix Physics MathCore Net)
#find_package(ROOT REQUIRED COMPONENTS Core)

#---Locate cppzmq package
find_package(cppzmq)

#---Define useful ROOT functions and macros (e.g. ROOT_GENERATE_DICTIONARY)
#include(${ROOT_USE_FILE})

#include_directories(${CMAKE_SOURCE_DIR} ${ROOT_INCLUDE_DIRS})
#add_definitions(${ROOT_CXX_FLAGS})

set(BUILD_CXX_FLAGS "-std=c++11 -pipe -fsigned-char -pthread")
add_definitions(${BUILD_CXX_FLAGS})

set(htmldir ${CMAKE_INSTALL_PREFIX}/share/html)
set (html_INDEX control.html)

add_definitions(-DHTMLFILE=\"${html_INDEX}\" -DHTMLDIR=\"${htmldir}\")

#add_definitions(-DPUSHPULL=ON)

set(CMAKE_EXE_LINKER_FLAGS "-Wl,-export-dynamic")

#print_all_variables()

#---Create a shared library with geneated dictionary
#ROOT_GENERATE_DICTIONARY(G__fitF ${PROJECT_SOURCE_DIR}/include/fitF.hh LINKDEF ${PROJECT_SOURCE_DIR}/include/fitFLinkDef.hh)
#add_library(fitF SHARED ${PROJECT_SOURCE_DIR}/src/fitF.cc ${PROJECT_SOURCE_DIR}/src/fitF_auxiliary.cc ${PROJECT_SOURCE_DIR}/src/fitF_cal.cc G__fitF.cxx)
#target_link_libraries(fitF ${ROOT_LIBRARIES})


add_library(eloghandler SHARED
    ${PROJECT_SOURCE_DIR}/include/eloghandler.h
    ${PROJECT_SOURCE_DIR}/src/eloghandler.cc)

add_library(rcdaq SHARED
    ${PROJECT_SOURCE_DIR}/include/rcdaq.h
    ${PROJECT_SOURCE_DIR}/include/daqEvent.h
    ${PROJECT_SOURCE_DIR}/include/daqONCSEvent.h
    ${PROJECT_SOURCE_DIR}/include/daqPRDFEvent.h
    ${PROJECT_SOURCE_DIR}/include/daq_device_random.h
    ${PROJECT_SOURCE_DIR}/include/daq_device_deadtime.h
    ${PROJECT_SOURCE_DIR}/include/daq_device_file.h
    ${PROJECT_SOURCE_DIR}/include/daq_device_filenumbers.h
    ${PROJECT_SOURCE_DIR}/include/daq_device_command.h
    ${PROJECT_SOURCE_DIR}/include/daq_device_rtclock.h
    ${PROJECT_SOURCE_DIR}/include/daqBuffer.h
    ${PROJECT_SOURCE_DIR}/include/daq_device.h
    ${PROJECT_SOURCE_DIR}/include/mongoose.h
    ${PROJECT_SOURCE_DIR}/include/parseargument.h
    ${PROJECT_SOURCE_DIR}/src/rcdaq.cc
    ${PROJECT_SOURCE_DIR}/src/daqEvent.cc
    ${PROJECT_SOURCE_DIR}/src/daqONCSEvent.cc
    ${PROJECT_SOURCE_DIR}/src/daqPRDFEvent.cc
    ${PROJECT_SOURCE_DIR}/src/daq_device_random.cc
    ${PROJECT_SOURCE_DIR}/src/daq_device_deadtime.cc
    ${PROJECT_SOURCE_DIR}/src/daq_device_file.cc
    ${PROJECT_SOURCE_DIR}/src/daq_device_filenumbers.cc
    ${PROJECT_SOURCE_DIR}/src/daq_device_command.cc
    ${PROJECT_SOURCE_DIR}/src/daq_device_rtclock.cc
    ${PROJECT_SOURCE_DIR}/src/daqBuffer.cc
    ${PROJECT_SOURCE_DIR}/src/daq_device.cc
    ${PROJECT_SOURCE_DIR}/src/rcdaq_mg_server.cc
    ${PROJECT_SOURCE_DIR}/src/mongoose.cc
    ${PROJECT_SOURCE_DIR}/src/parseargument.cc)
target_link_libraries(rcdaq eloghandler)

add_library(rcdaqplugin_example SHARED
    ${PROJECT_SOURCE_DIR}/include/daq_device_pluginexample.h
    ${PROJECT_SOURCE_DIR}/include/example_plugin.h
    ${PROJECT_SOURCE_DIR}/src/daq_device_pluginexample.cc
    ${PROJECT_SOURCE_DIR}/src/example_plugin.cc)

add_library(rcdaqplugin_gauss SHARED
    ${PROJECT_SOURCE_DIR}/include/daq_device_gauss.h
    ${PROJECT_SOURCE_DIR}/include/gauss_plugin.h
    ${PROJECT_SOURCE_DIR}/include/simpleRandom.h
    ${PROJECT_SOURCE_DIR}/src/daq_device_gauss.cc
    ${PROJECT_SOURCE_DIR}/src/gauss_plugin.cc
    ${PROJECT_SOURCE_DIR}/src/simpleRandom.cc)

add_library(rcdaqutils SHARED
    ${PROJECT_SOURCE_DIR}/include/parseargument.h
    ${PROJECT_SOURCE_DIR}/src/parseargument.cc)

#include source and include dir
include_directories(${PROJECT_SOURCE_DIR}/include/)


#---Create  a main program using the library
add_executable(rcdaq_server    
    rcdaq_server.cc)
target_link_libraries(rcdaq_server rcdaq)
target_link_libraries(rcdaq_server dl)
target_link_libraries(rcdaq_server pthread)
target_link_libraries(rcdaq_server cppzmq)

add_executable(rcdaq_client
    rcdaq_client.cc)
target_link_libraries(rcdaq_client cppzmq)

add_executable(elogtest
    elogtest.cc)
target_link_libraries(elogtest eloghandler)

add_executable(datasink
    datasink.cc)
target_link_libraries(datasink cppzmq)
target_link_libraries(datasink dl)
target_link_libraries(datasink pthread)

add_executable(onlinetest
    onlinetest.cc)
target_link_libraries(onlinetest cppzmq)

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

file (WRITE setup.sh "export ONLINE_MAIN=" ${CMAKE_INSTALL_PREFIX} "\nexport OFFLINE_MAIN=" ${CMAKE_INSTALL_PREFIX} "\nexport PATH=$ONLINE_MAIN/bin:$PATH\nexport LD_LIBRARY_PATH=$ONLINE_MAIN/lib\nsource $ONLINE_MAIN/bin/aliases.sh\nexport ROOT_INCLUDE_PATH=$ONLINE_MAIN/include:$ONLINE_MAIN/include/Event:$ONLINE_MAIN/include/pmonitor")
file (COPY setup.sh DESTINATION ${CMAKE_BINARY_DIR} FILE_PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ)

set(rcdaqheaders ${PROJECT_SOURCE_DIR}/include/SubevtStructures.h
    ${PROJECT_SOURCE_DIR}/include/SubevtConstants.h
    ${PROJECT_SOURCE_DIR}/include/daq_device.h
    ${PROJECT_SOURCE_DIR}/include/TriggerHandler.h
    ${PROJECT_SOURCE_DIR}/include/deviceblock.h
    ${PROJECT_SOURCE_DIR}/include/rcdaq_plugin.h
    ${PROJECT_SOURCE_DIR}/include/parseargument.h)
set_target_properties(rcdaq PROPERTIES PUBLIC_HEADER "${rcdaqheaders}")
install(TARGETS rcdaq
        LIBRARY DESTINATION lib
        PUBLIC_HEADER DESTINATION include)

install(TARGETS rcdaqplugin_example
        LIBRARY DESTINATION lib)

install(TARGETS rcdaqplugin_gauss
        LIBRARY DESTINATION lib)

install(TARGETS rcdaqutils
        LIBRARY DESTINATION lib)

install(TARGETS eloghandler
    LIBRARY DESTINATION lib)

install (TARGETS rcdaq_server DESTINATION bin)

install (TARGETS rcdaq_client DESTINATION bin)

install (TARGETS datasink DESTINATION bin)

install (TARGETS onlinetest DESTINATION bin)

install (TARGETS elogtest DESTINATION bin)

set(ELOG_RESOURCE_FILES
    control.html
    favicon.ico)
install(
    FILES ${ELOG_RESOURCE_FILES}
    DESTINATION share/html)

set(BIN_RESOURCE_FILES
    setup.sh
    rcdaq_control.pl
    rcdaq_configmenu.pl
    automated_run_example.sh
    aliases.sh
    aliases.csh
    daq_select
    wait_for_run_end.sh
    rcdaq_status.pl
    rcdaq_runtypechooser.pl)
install(
    FILES ${BIN_RESOURCE_FILES}
    PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ
    DESTINATION bin
)


