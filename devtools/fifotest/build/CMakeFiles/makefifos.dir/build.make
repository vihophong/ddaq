# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/local/bin/cmake

# The command to remove a file.
RM = /usr/local/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/phong/projects/ddaq/ddaq-git/ddaq_earth/devtools

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/phong/projects/ddaq/ddaq-git/ddaq_earth/devtools/build

# Include any dependencies generated for this target.
include CMakeFiles/makefifos.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/makefifos.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/makefifos.dir/flags.make

CMakeFiles/makefifos.dir/makefifos.cpp.o: CMakeFiles/makefifos.dir/flags.make
CMakeFiles/makefifos.dir/makefifos.cpp.o: ../makefifos.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/phong/projects/ddaq/ddaq-git/ddaq_earth/devtools/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/makefifos.dir/makefifos.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/makefifos.dir/makefifos.cpp.o -c /home/phong/projects/ddaq/ddaq-git/ddaq_earth/devtools/makefifos.cpp

CMakeFiles/makefifos.dir/makefifos.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/makefifos.dir/makefifos.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/phong/projects/ddaq/ddaq-git/ddaq_earth/devtools/makefifos.cpp > CMakeFiles/makefifos.dir/makefifos.cpp.i

CMakeFiles/makefifos.dir/makefifos.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/makefifos.dir/makefifos.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/phong/projects/ddaq/ddaq-git/ddaq_earth/devtools/makefifos.cpp -o CMakeFiles/makefifos.dir/makefifos.cpp.s

# Object files for target makefifos
makefifos_OBJECTS = \
"CMakeFiles/makefifos.dir/makefifos.cpp.o"

# External object files for target makefifos
makefifos_EXTERNAL_OBJECTS =

makefifos: CMakeFiles/makefifos.dir/makefifos.cpp.o
makefifos: CMakeFiles/makefifos.dir/build.make
makefifos: CMakeFiles/makefifos.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/phong/projects/ddaq/ddaq-git/ddaq_earth/devtools/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable makefifos"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/makefifos.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/makefifos.dir/build: makefifos

.PHONY : CMakeFiles/makefifos.dir/build

CMakeFiles/makefifos.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/makefifos.dir/cmake_clean.cmake
.PHONY : CMakeFiles/makefifos.dir/clean

CMakeFiles/makefifos.dir/depend:
	cd /home/phong/projects/ddaq/ddaq-git/ddaq_earth/devtools/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/phong/projects/ddaq/ddaq-git/ddaq_earth/devtools /home/phong/projects/ddaq/ddaq-git/ddaq_earth/devtools /home/phong/projects/ddaq/ddaq-git/ddaq_earth/devtools/build /home/phong/projects/ddaq/ddaq-git/ddaq_earth/devtools/build /home/phong/projects/ddaq/ddaq-git/ddaq_earth/devtools/build/CMakeFiles/makefifos.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/makefifos.dir/depend

