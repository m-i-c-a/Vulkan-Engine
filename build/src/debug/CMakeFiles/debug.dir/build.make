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
CMAKE_SOURCE_DIR = /home/mica/Desktop/Repos/Vulkan-Engine

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/mica/Desktop/Repos/Vulkan-Engine/build

# Include any dependencies generated for this target.
include src/debug/CMakeFiles/debug.dir/depend.make

# Include the progress variables for this target.
include src/debug/CMakeFiles/debug.dir/progress.make

# Include the compile flags for this target's objects.
include src/debug/CMakeFiles/debug.dir/flags.make

src/debug/CMakeFiles/debug.dir/DebugUtilsEXT.cpp.o: src/debug/CMakeFiles/debug.dir/flags.make
src/debug/CMakeFiles/debug.dir/DebugUtilsEXT.cpp.o: ../src/debug/DebugUtilsEXT.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/mica/Desktop/Repos/Vulkan-Engine/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object src/debug/CMakeFiles/debug.dir/DebugUtilsEXT.cpp.o"
	cd /home/mica/Desktop/Repos/Vulkan-Engine/build/src/debug && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/debug.dir/DebugUtilsEXT.cpp.o -c /home/mica/Desktop/Repos/Vulkan-Engine/src/debug/DebugUtilsEXT.cpp

src/debug/CMakeFiles/debug.dir/DebugUtilsEXT.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/debug.dir/DebugUtilsEXT.cpp.i"
	cd /home/mica/Desktop/Repos/Vulkan-Engine/build/src/debug && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/mica/Desktop/Repos/Vulkan-Engine/src/debug/DebugUtilsEXT.cpp > CMakeFiles/debug.dir/DebugUtilsEXT.cpp.i

src/debug/CMakeFiles/debug.dir/DebugUtilsEXT.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/debug.dir/DebugUtilsEXT.cpp.s"
	cd /home/mica/Desktop/Repos/Vulkan-Engine/build/src/debug && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/mica/Desktop/Repos/Vulkan-Engine/src/debug/DebugUtilsEXT.cpp -o CMakeFiles/debug.dir/DebugUtilsEXT.cpp.s

# Object files for target debug
debug_OBJECTS = \
"CMakeFiles/debug.dir/DebugUtilsEXT.cpp.o"

# External object files for target debug
debug_EXTERNAL_OBJECTS =

src/debug/libdebug.a: src/debug/CMakeFiles/debug.dir/DebugUtilsEXT.cpp.o
src/debug/libdebug.a: src/debug/CMakeFiles/debug.dir/build.make
src/debug/libdebug.a: src/debug/CMakeFiles/debug.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/mica/Desktop/Repos/Vulkan-Engine/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX static library libdebug.a"
	cd /home/mica/Desktop/Repos/Vulkan-Engine/build/src/debug && $(CMAKE_COMMAND) -P CMakeFiles/debug.dir/cmake_clean_target.cmake
	cd /home/mica/Desktop/Repos/Vulkan-Engine/build/src/debug && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/debug.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/debug/CMakeFiles/debug.dir/build: src/debug/libdebug.a

.PHONY : src/debug/CMakeFiles/debug.dir/build

src/debug/CMakeFiles/debug.dir/clean:
	cd /home/mica/Desktop/Repos/Vulkan-Engine/build/src/debug && $(CMAKE_COMMAND) -P CMakeFiles/debug.dir/cmake_clean.cmake
.PHONY : src/debug/CMakeFiles/debug.dir/clean

src/debug/CMakeFiles/debug.dir/depend:
	cd /home/mica/Desktop/Repos/Vulkan-Engine/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/mica/Desktop/Repos/Vulkan-Engine /home/mica/Desktop/Repos/Vulkan-Engine/src/debug /home/mica/Desktop/Repos/Vulkan-Engine/build /home/mica/Desktop/Repos/Vulkan-Engine/build/src/debug /home/mica/Desktop/Repos/Vulkan-Engine/build/src/debug/CMakeFiles/debug.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/debug/CMakeFiles/debug.dir/depend

