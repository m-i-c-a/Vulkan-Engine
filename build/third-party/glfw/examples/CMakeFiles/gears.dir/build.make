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
include third-party/glfw/examples/CMakeFiles/gears.dir/depend.make

# Include the progress variables for this target.
include third-party/glfw/examples/CMakeFiles/gears.dir/progress.make

# Include the compile flags for this target's objects.
include third-party/glfw/examples/CMakeFiles/gears.dir/flags.make

third-party/glfw/examples/CMakeFiles/gears.dir/gears.c.o: third-party/glfw/examples/CMakeFiles/gears.dir/flags.make
third-party/glfw/examples/CMakeFiles/gears.dir/gears.c.o: ../third-party/glfw/examples/gears.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/mica/Desktop/Vulkan/Vulkan-Engine/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object third-party/glfw/examples/CMakeFiles/gears.dir/gears.c.o"
	cd /home/mica/Desktop/Vulkan/Vulkan-Engine/build/third-party/glfw/examples && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/gears.dir/gears.c.o   -c /home/mica/Desktop/Vulkan/Vulkan-Engine/third-party/glfw/examples/gears.c

third-party/glfw/examples/CMakeFiles/gears.dir/gears.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/gears.dir/gears.c.i"
	cd /home/mica/Desktop/Vulkan/Vulkan-Engine/build/third-party/glfw/examples && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/mica/Desktop/Vulkan/Vulkan-Engine/third-party/glfw/examples/gears.c > CMakeFiles/gears.dir/gears.c.i

third-party/glfw/examples/CMakeFiles/gears.dir/gears.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/gears.dir/gears.c.s"
	cd /home/mica/Desktop/Vulkan/Vulkan-Engine/build/third-party/glfw/examples && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/mica/Desktop/Vulkan/Vulkan-Engine/third-party/glfw/examples/gears.c -o CMakeFiles/gears.dir/gears.c.s

# Object files for target gears
gears_OBJECTS = \
"CMakeFiles/gears.dir/gears.c.o"

# External object files for target gears
gears_EXTERNAL_OBJECTS =

third-party/glfw/examples/gears: third-party/glfw/examples/CMakeFiles/gears.dir/gears.c.o
third-party/glfw/examples/gears: third-party/glfw/examples/CMakeFiles/gears.dir/build.make
third-party/glfw/examples/gears: third-party/glfw/src/libglfw3.a
third-party/glfw/examples/gears: /usr/lib/x86_64-linux-gnu/libm.so
third-party/glfw/examples/gears: /usr/lib/x86_64-linux-gnu/librt.so
third-party/glfw/examples/gears: /usr/lib/x86_64-linux-gnu/libm.so
third-party/glfw/examples/gears: third-party/glfw/examples/CMakeFiles/gears.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/mica/Desktop/Vulkan/Vulkan-Engine/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable gears"
	cd /home/mica/Desktop/Vulkan/Vulkan-Engine/build/third-party/glfw/examples && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/gears.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
third-party/glfw/examples/CMakeFiles/gears.dir/build: third-party/glfw/examples/gears

.PHONY : third-party/glfw/examples/CMakeFiles/gears.dir/build

third-party/glfw/examples/CMakeFiles/gears.dir/clean:
	cd /home/mica/Desktop/Vulkan/Vulkan-Engine/build/third-party/glfw/examples && $(CMAKE_COMMAND) -P CMakeFiles/gears.dir/cmake_clean.cmake
.PHONY : third-party/glfw/examples/CMakeFiles/gears.dir/clean

third-party/glfw/examples/CMakeFiles/gears.dir/depend:
	cd /home/mica/Desktop/Vulkan/Vulkan-Engine/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/mica/Desktop/Vulkan/Vulkan-Engine /home/mica/Desktop/Vulkan/Vulkan-Engine/third-party/glfw/examples /home/mica/Desktop/Vulkan/Vulkan-Engine/build /home/mica/Desktop/Vulkan/Vulkan-Engine/build/third-party/glfw/examples /home/mica/Desktop/Vulkan/Vulkan-Engine/build/third-party/glfw/examples/CMakeFiles/gears.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : third-party/glfw/examples/CMakeFiles/gears.dir/depend

