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
project(newbasic)

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

find_package(Boost 1.45.0)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
endif()

# You need to tell CMake where to find the ROOT installation. This can be done in a number of ways:
#   - ROOT built with classic configure/make use the provided $ROOTSYS/etc/cmake/FindROOT.cmake
#   - ROOT built with CMake. Add in CMAKE_PREFIX_PATH the installation prefix for ROOT
list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})

#---Locate the ROOT package and defines a number of variables (e.g. ROOT_INCLUDE_DIRS)
find_package(ROOT REQUIRED COMPONENTS Core RIO Hist Tree Matrix Physics MathCore Net)

#---Locate cppzmq package
find_package(cppzmq)
if (cppzmq_FOUND)
    add_definitions(-DCPPZMQ=1)
endif()

#---Define useful ROOT functions and macros (e.g. ROOT_GENERATE_DICTIONARY)
include(${ROOT_USE_FILE})

include_directories(${CMAKE_SOURCE_DIR} ${ROOT_INCLUDE_DIRS})
add_definitions(${ROOT_CXX_FLAGS})

add_definitions(-DRDBUF_ACCEPTS_STREAMBUF=1 -DHAVE_STRPTIME_PROTOTYPE=1 -DHAVE_GETOPT_H=1 -DLinux=1 -DHAVE_ROOT6=1)

#set(BUILD_CXX_FLAGS2 "-lz")
#add_definitions(${BUILD_CXX_FLAGS2})

#print_all_variables()

#include source and include dir
include_directories(${PROJECT_SOURCE_DIR}/include/Event)
include_directories(${PROJECT_SOURCE_DIR}/include)
if (cppzmq_FOUND)
    include_directories(${cppzmq_INCLUDE_DIR})
    message(STATUS "Included directory: ${cppzmq_INCLUDE_DIR} for ZMQ and CPPZMQ")
endif()


file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB sources_C ${PROJECT_SOURCE_DIR}/src/*.C)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/Event/*.h)

set(libmsg_sources ${PROJECT_SOURCE_DIR}/src/date_filter_msg_buffer.cc
    ${PROJECT_SOURCE_DIR}/src/filter_msg_buffer.cc
    ${PROJECT_SOURCE_DIR}/src/remote_msg_buffer.cc
    ${PROJECT_SOURCE_DIR}/src/msg_buffer.cc
    ${PROJECT_SOURCE_DIR}/src/msg_control.cc
    ${PROJECT_SOURCE_DIR}/src/strnstr.cc)

set(libmsg_headers ${PROJECT_SOURCE_DIR}/include/Event/date_filter_msg_buffer.h
    ${PROJECT_SOURCE_DIR}/include/Event/event_io.h
    ${PROJECT_SOURCE_DIR}/include/Event/filter_msg_buffer.h
    ${PROJECT_SOURCE_DIR}/include/Event/remote_msg_buffer.h
    ${PROJECT_SOURCE_DIR}/include/Event/msg_buffer.h
    ${PROJECT_SOURCE_DIR}/include/Event/msg_control.h
    ${PROJECT_SOURCE_DIR}/include/Event/strnstr.h)


add_library(message SHARED ${libmsg_headers} ${libmsg_sources})
#target_link_libraries(message ${ROOT_LIBRARIES})


set(msg_dict_files ${PROJECT_SOURCE_DIR}/include/Event/msg_control.h
    ${PROJECT_SOURCE_DIR}/include/Event/date_filter_msg_buffer.h
    ${PROJECT_SOURCE_DIR}/include/Event/filter_msg_buffer.h
    ${PROJECT_SOURCE_DIR}/include/Event/remote_msg_buffer.h)

set(Rootmessage_sources ${PROJECT_SOURCE_DIR}/src/date_filter_msg_buffer.cc
    ${PROJECT_SOURCE_DIR}/src/filter_msg_buffer.cc
    ${PROJECT_SOURCE_DIR}/src/msg_buffer.cc
    ${PROJECT_SOURCE_DIR}/src/msg_control.cc
    ${PROJECT_SOURCE_DIR}/src/remote_msg_buffer.cc
    ${PROJECT_SOURCE_DIR}/src/strnstr.cc)

set(Rootmessage_headers ${PROJECT_SOURCE_DIR}/include/Event/date_filter_msg_buffer.h
    ${PROJECT_SOURCE_DIR}/include/Event/filter_msg_buffer.h
    ${PROJECT_SOURCE_DIR}/include/Event/msg_buffer.h
    ${PROJECT_SOURCE_DIR}/include/Event/msg_control.h
    ${PROJECT_SOURCE_DIR}/include/Event/remote_msg_buffer.h
    ${PROJECT_SOURCE_DIR}/include/Event/strnstr.h)


#---Create a shared library with geneated dictionary
ROOT_GENERATE_DICTIONARY(G__msg_dict ${msg_dict_files} LINKDEF ${PROJECT_SOURCE_DIR}/include/Event/msgLinkDef.h)
add_library(Rootmessage SHARED ${Rootmessage_headers} ${Rootmessage_sources} G__msg_dict.cxx)
target_link_libraries(Rootmessage message)


#---Create a shared library
add_library(NoRootEvent SHARED ${headers} ${sources} ${sources_C})
target_link_libraries(NoRootEvent message)
#target_link_libraries(NoRootEvent z)
#target_link_libraries(NoRootEvent lzo2)#lzo
if (cppzmq_FOUND)
target_link_libraries(NoRootEvent cppzmq)
endif()

set(event_dict_files ${PROJECT_SOURCE_DIR}/include/Event/Event.h
    ${PROJECT_SOURCE_DIR}/include/Event/Eventiterator.h
    ${PROJECT_SOURCE_DIR}/include/Event/fileEventiterator.h
    ${PROJECT_SOURCE_DIR}/include/Event/listEventiterator.h
    ${PROJECT_SOURCE_DIR}/include/Event/oncsEventiterator.h
    ${PROJECT_SOURCE_DIR}/include/Event/rcdaqEventiterator.h
    ${PROJECT_SOURCE_DIR}/include/Event/packet.h
    ${PROJECT_SOURCE_DIR}/include/Event/testEventiterator.h)

#---Create a shared library with geneated dictionary
ROOT_GENERATE_DICTIONARY(G__event_dict ${event_dict_files} LINKDEF ${PROJECT_SOURCE_DIR}/include/Event/eventLinkDef.h)
add_library(Event SHARED ${Rootmessage_headers} ${Rootmessage_sources} G__event_dict.cxx)
target_link_libraries(Event NoRootEvent)
target_link_libraries(Event Rootmessage)
target_link_libraries(Event ${ROOT_LIBRARIES})
#target_link_libraries(Event z)
#target_link_libraries(Event lzo2)


set(pmonitor_sources ${PROJECT_SOURCE_DIR}/pmonitor_src/pmonitor.cc
    ${PROJECT_SOURCE_DIR}/pmonitor_src/pMutex.cc)

set(pmonitor_headers ${PROJECT_SOURCE_DIR}/include/pmonitor/pmonitor.h
    ${PROJECT_SOURCE_DIR}/include/pmonitor/pmonitorLinkDef.h
    ${PROJECT_SOURCE_DIR}/include/pmonitor/pmonstate.h
    ${PROJECT_SOURCE_DIR}/include/pmonitor/pMutex.h)
#---Create a shared library with geneated dictionary
ROOT_GENERATE_DICTIONARY(G__pmonitor ${PROJECT_SOURCE_DIR}/include/pmonitor/pmonitor.h LINKDEF ${PROJECT_SOURCE_DIR}/include/pmonitor/pmonitorLinkDef.h)
add_library(pmonitor SHARED ${pmonitor_sources} ${pmonitor_headers}  G__pmonitor.cxx)
target_link_libraries(pmonitor Event NoRootEvent message)
target_link_libraries(pmonitor ${ROOT_LIBRARIES})


#-- Excutable tools
add_executable(dlist
    dlist.cc)
target_link_libraries(dlist NoRootEvent)
target_link_libraries(dlist dl)
#target_link_libraries(dlist z)
#target_link_libraries(dlist lzo2)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    add_executable(ddump
        ddump.cc)
    target_link_libraries(ddump NoRootEvent)
    target_link_libraries(ddump dl)
    #target_link_libraries(ddump z)
    #target_link_libraries(ddump lzo2)
endif()



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

file (WRITE setuponline.sh "export ONLINE_MAIN=" ${CMAKE_INSTALL_PREFIX} "\nexport OFFLINE_MAIN=" ${CMAKE_INSTALL_PREFIX} "\nexport PATH=$ONLINE_MAIN/bin:$PATH\nexport LD_LIBRARY_PATH=$ONLINE_MAIN/lib\nexport ROOT_INCLUDE_PATH=$ONLINE_MAIN/include:$ONLINE_MAIN/include/Event:$ONLINE_MAIN/include/pmonitor")
file (COPY setuponline.sh DESTINATION ${CMAKE_BINARY_DIR} FILE_PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ)

install(TARGETS message
        LIBRARY DESTINATION lib
)

install(TARGETS Rootmessage
        LIBRARY DESTINATION lib
)
install (FILES ${PROJECT_BINARY_DIR}/libmsg_dict_rdict.pcm DESTINATION lib)

install(TARGETS NoRootEvent
        LIBRARY DESTINATION lib
)

set(eventheaders
    ${PROJECT_SOURCE_DIR}/include/Event/BufferConstants.h
    ${PROJECT_SOURCE_DIR}/include/Event/buffer.h
    ${PROJECT_SOURCE_DIR}/include/Event/Cpacket.h
    ${PROJECT_SOURCE_DIR}/include/Event/dataBlock.h
    ${PROJECT_SOURCE_DIR}/include/Event/date_filter_msg_buffer.h
    ${PROJECT_SOURCE_DIR}/include/Event/decoding_routines.h
    ${PROJECT_SOURCE_DIR}/include/Event/errorBlock.h
    ${PROJECT_SOURCE_DIR}/include/Event/Event.h
    ${PROJECT_SOURCE_DIR}/include/Event/event_io.h
    ${PROJECT_SOURCE_DIR}/include/Event/Eventiterator.h
    ${PROJECT_SOURCE_DIR}/include/Event/EventTypes.h
    ${PROJECT_SOURCE_DIR}/include/Event/EvtConstants.h
    ${PROJECT_SOURCE_DIR}/include/Event/EvtStructures.h
    ${PROJECT_SOURCE_DIR}/include/Event/fileEventiterator.h
    ${PROJECT_SOURCE_DIR}/include/Event/filter_msg_buffer.h
    ${PROJECT_SOURCE_DIR}/include/Event/formatError.h
    ${PROJECT_SOURCE_DIR}/include/Event/framePackets.h
    ${PROJECT_SOURCE_DIR}/include/Event/framePublic.h
    ${PROJECT_SOURCE_DIR}/include/Event/frameV1Public.h
    ${PROJECT_SOURCE_DIR}/include/Event/generalDefinitions.h
    ${PROJECT_SOURCE_DIR}/include/Event/ioselect.h
    ${PROJECT_SOURCE_DIR}/include/Event/listEventiterator.h
    ${PROJECT_SOURCE_DIR}/include/Event/msg_buffer.h
    ${PROJECT_SOURCE_DIR}/include/Event/msg_control.h
    ${PROJECT_SOURCE_DIR}/include/Event/msg_profile.h
    ${PROJECT_SOURCE_DIR}/include/Event/oncsBuffer.h
    ${PROJECT_SOURCE_DIR}/include/Event/oncsEvent.h
    ${PROJECT_SOURCE_DIR}/include/Event/oncsEventiterator.h
    ${PROJECT_SOURCE_DIR}/include/Event/oncsEvtConstants.h
    ${PROJECT_SOURCE_DIR}/include/Event/oncsEvtStructures.h
    ${PROJECT_SOURCE_DIR}/include/Event/oncsStructures.h
    ${PROJECT_SOURCE_DIR}/include/Event/oncsSubConstants.h
    ${PROJECT_SOURCE_DIR}/include/Event/packet.h
    ${PROJECT_SOURCE_DIR}/include/Event/phenixOnline.h
    ${PROJECT_SOURCE_DIR}/include/Event/phenixTypes.h
    ${PROJECT_SOURCE_DIR}/include/Event/rcdaqEventiterator.h
    ${PROJECT_SOURCE_DIR}/include/Event/remote_msg_buffer.h
    ${PROJECT_SOURCE_DIR}/include/Event/simpleRandom.h
    ${PROJECT_SOURCE_DIR}/include/Event/strnstr.h
    ${PROJECT_SOURCE_DIR}/include/Event/testEventiterator.h)
set_target_properties(Event PROPERTIES PUBLIC_HEADER "${eventheaders}")
install(TARGETS Event
        LIBRARY DESTINATION lib
        PUBLIC_HEADER DESTINATION include/Event
)
install (FILES ${PROJECT_BINARY_DIR}/libevent_dict_rdict.pcm DESTINATION lib)

#pmonitor
set_target_properties(pmonitor PROPERTIES PUBLIC_HEADER "${pmonitor_headers}")
install(TARGETS pmonitor
        LIBRARY DESTINATION lib
        PUBLIC_HEADER DESTINATION include/pmonitor
)
install (FILES ${PROJECT_BINARY_DIR}/libpmonitor_rdict.pcm DESTINATION lib)

#binary installation
install(TARGETS dlist DESTINATION bin)
if(Boost_FOUND)
    install(TARGETS ddump DESTINATION bin)
endif()

install(
    FILES setuponline.sh
    PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ
    DESTINATION bin
)
