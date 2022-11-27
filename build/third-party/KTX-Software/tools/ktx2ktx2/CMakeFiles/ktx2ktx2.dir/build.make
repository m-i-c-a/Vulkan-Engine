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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/mica/Desktop/Vulkan/Vulkan-Engine

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/mica/Desktop/Vulkan/Vulkan-Engine/build

# Include any dependencies generated for this target.
include third-party/KTX-Software/tools/ktx2ktx2/CMakeFiles/ktx2ktx2.dir/depend.make

# Include the progress variables for this target.
include third-party/KTX-Software/tools/ktx2ktx2/CMakeFiles/ktx2ktx2.dir/progress.make

# Include the compile flags for this target's objects.
include third-party/KTX-Software/tools/ktx2ktx2/CMakeFiles/ktx2ktx2.dir/flags.make

../third-party/KTX-Software/tools/ktx2ktx2/version.h:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/mica/Desktop/Vulkan/Vulkan-Engine/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Generate /home/mica/Desktop/Vulkan/Vulkan-Engine/third-party/KTX-Software/tools/ktx2ktx2/version.h"
	cd /home/mica/Desktop/Vulkan/Vulkan-Engine/third-party/KTX-Software && ./mkversion -o version.h tools/ktx2ktx2

third-party/KTX-Software/tools/ktx2ktx2/CMakeFiles/ktx2ktx2.dir/ktx2ktx2.cpp.o: third-party/KTX-Software/tools/ktx2ktx2/CMakeFiles/ktx2ktx2.dir/flags.make
third-party/KTX-Software/tools/ktx2ktx2/CMakeFiles/ktx2ktx2.dir/ktx2ktx2.cpp.o: ../third-party/KTX-Software/tools/ktx2ktx2/ktx2ktx2.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/mica/Desktop/Vulkan/Vulkan-Engine/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object third-party/KTX-Software/tools/ktx2ktx2/CMakeFiles/ktx2ktx2.dir/ktx2ktx2.cpp.o"
	cd /home/mica/Desktop/Vulkan/Vulkan-Engine/build/third-party/KTX-Software/tools/ktx2ktx2 && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/ktx2ktx2.dir/ktx2ktx2.cpp.o -c /home/mica/Desktop/Vulkan/Vulkan-Engine/third-party/KTX-Software/tools/ktx2ktx2/ktx2ktx2.cpp

third-party/KTX-Software/tools/ktx2ktx2/CMakeFiles/ktx2ktx2.dir/ktx2ktx2.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/ktx2ktx2.dir/ktx2ktx2.cpp.i"
	cd /home/mica/Desktop/Vulkan/Vulkan-Engine/build/third-party/KTX-Software/tools/ktx2ktx2 && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/mica/Desktop/Vulkan/Vulkan-Engine/third-party/KTX-Software/tools/ktx2ktx2/ktx2ktx2.cpp > CMakeFiles/ktx2ktx2.dir/ktx2ktx2.cpp.i

third-party/KTX-Software/tools/ktx2ktx2/CMakeFiles/ktx2ktx2.dir/ktx2ktx2.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/ktx2ktx2.dir/ktx2ktx2.cpp.s"
	cd /home/mica/Desktop/Vulkan/Vulkan-Engine/build/third-party/KTX-Software/tools/ktx2ktx2 && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/mica/Desktop/Vulkan/Vulkan-Engine/third-party/KTX-Software/tools/ktx2ktx2/ktx2ktx2.cpp -o CMakeFiles/ktx2ktx2.dir/ktx2ktx2.cpp.s

# Object files for target ktx2ktx2
ktx2ktx2_OBJECTS = \
"CMakeFiles/ktx2ktx2.dir/ktx2ktx2.cpp.o"

# External object files for target ktx2ktx2
ktx2ktx2_EXTERNAL_OBJECTS =

third-party/KTX-Software/tools/ktx2ktx2/ktx2ktx2: third-party/KTX-Software/tools/ktx2ktx2/CMakeFiles/ktx2ktx2.dir/ktx2ktx2.cpp.o
third-party/KTX-Software/tools/ktx2ktx2/ktx2ktx2: third-party/KTX-Software/tools/ktx2ktx2/CMakeFiles/ktx2ktx2.dir/build.make
third-party/KTX-Software/tools/ktx2ktx2/ktx2ktx2: third-party/KTX-Software/libktx.so.4.1.0
third-party/KTX-Software/tools/ktx2ktx2/ktx2ktx2: third-party/KTX-Software/libobjUtil.a
third-party/KTX-Software/tools/ktx2ktx2/ktx2ktx2: third-party/KTX-Software/tools/ktx2ktx2/CMakeFiles/ktx2ktx2.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/mica/Desktop/Vulkan/Vulkan-Engine/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX executable ktx2ktx2"
	cd /home/mica/Desktop/Vulkan/Vulkan-Engine/build/third-party/KTX-Software/tools/ktx2ktx2 && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/ktx2ktx2.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
third-party/KTX-Software/tools/ktx2ktx2/CMakeFiles/ktx2ktx2.dir/build: third-party/KTX-Software/tools/ktx2ktx2/ktx2ktx2

.PHONY : third-party/KTX-Software/tools/ktx2ktx2/CMakeFiles/ktx2ktx2.dir/build

third-party/KTX-Software/tools/ktx2ktx2/CMakeFiles/ktx2ktx2.dir/clean:
	cd /home/mica/Desktop/Vulkan/Vulkan-Engine/build/third-party/KTX-Software/tools/ktx2ktx2 && $(CMAKE_COMMAND) -P CMakeFiles/ktx2ktx2.dir/cmake_clean.cmake
.PHONY : third-party/KTX-Software/tools/ktx2ktx2/CMakeFiles/ktx2ktx2.dir/clean

third-party/KTX-Software/tools/ktx2ktx2/CMakeFiles/ktx2ktx2.dir/depend: ../third-party/KTX-Software/tools/ktx2ktx2/version.h
	cd /home/mica/Desktop/Vulkan/Vulkan-Engine/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/mica/Desktop/Vulkan/Vulkan-Engine /home/mica/Desktop/Vulkan/Vulkan-Engine/third-party/KTX-Software/tools/ktx2ktx2 /home/mica/Desktop/Vulkan/Vulkan-Engine/build /home/mica/Desktop/Vulkan/Vulkan-Engine/build/third-party/KTX-Software/tools/ktx2ktx2 /home/mica/Desktop/Vulkan/Vulkan-Engine/build/third-party/KTX-Software/tools/ktx2ktx2/CMakeFiles/ktx2ktx2.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : third-party/KTX-Software/tools/ktx2ktx2/CMakeFiles/ktx2ktx2.dir/depend

