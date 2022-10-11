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
CMAKE_SOURCE_DIR = /home/mica/Desktop/Engine

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/mica/Desktop/Engine/build

# Include any dependencies generated for this target.
include CMakeFiles/engine.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/engine.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/engine.dir/flags.make

CMakeFiles/engine.dir/main.cpp.o: CMakeFiles/engine.dir/flags.make
CMakeFiles/engine.dir/main.cpp.o: ../main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/mica/Desktop/Engine/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/engine.dir/main.cpp.o"
	/usr/bin/g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/engine.dir/main.cpp.o -c /home/mica/Desktop/Engine/main.cpp

CMakeFiles/engine.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/engine.dir/main.cpp.i"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/mica/Desktop/Engine/main.cpp > CMakeFiles/engine.dir/main.cpp.i

CMakeFiles/engine.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/engine.dir/main.cpp.s"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/mica/Desktop/Engine/main.cpp -o CMakeFiles/engine.dir/main.cpp.s

CMakeFiles/engine.dir/VulkanCore.cpp.o: CMakeFiles/engine.dir/flags.make
CMakeFiles/engine.dir/VulkanCore.cpp.o: ../VulkanCore.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/mica/Desktop/Engine/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/engine.dir/VulkanCore.cpp.o"
	/usr/bin/g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/engine.dir/VulkanCore.cpp.o -c /home/mica/Desktop/Engine/VulkanCore.cpp

CMakeFiles/engine.dir/VulkanCore.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/engine.dir/VulkanCore.cpp.i"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/mica/Desktop/Engine/VulkanCore.cpp > CMakeFiles/engine.dir/VulkanCore.cpp.i

CMakeFiles/engine.dir/VulkanCore.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/engine.dir/VulkanCore.cpp.s"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/mica/Desktop/Engine/VulkanCore.cpp -o CMakeFiles/engine.dir/VulkanCore.cpp.s

# Object files for target engine
engine_OBJECTS = \
"CMakeFiles/engine.dir/main.cpp.o" \
"CMakeFiles/engine.dir/VulkanCore.cpp.o"

# External object files for target engine
engine_EXTERNAL_OBJECTS =

engine: CMakeFiles/engine.dir/main.cpp.o
engine: CMakeFiles/engine.dir/VulkanCore.cpp.o
engine: CMakeFiles/engine.dir/build.make
engine: third-party/glfw/src/libglfw3.a
engine: /home/mica/Desktop/Vulkan/1.3.224.1/x86_64/lib/libvulkan.so
engine: /usr/lib/x86_64-linux-gnu/librt.so
engine: /usr/lib/x86_64-linux-gnu/libm.so
engine: CMakeFiles/engine.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/mica/Desktop/Engine/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX executable engine"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/engine.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/engine.dir/build: engine

.PHONY : CMakeFiles/engine.dir/build

CMakeFiles/engine.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/engine.dir/cmake_clean.cmake
.PHONY : CMakeFiles/engine.dir/clean

CMakeFiles/engine.dir/depend:
	cd /home/mica/Desktop/Engine/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/mica/Desktop/Engine /home/mica/Desktop/Engine /home/mica/Desktop/Engine/build /home/mica/Desktop/Engine/build /home/mica/Desktop/Engine/build/CMakeFiles/engine.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/engine.dir/depend

