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
include third-party/glfw/examples/CMakeFiles/particles.dir/depend.make

# Include the progress variables for this target.
include third-party/glfw/examples/CMakeFiles/particles.dir/progress.make

# Include the compile flags for this target's objects.
include third-party/glfw/examples/CMakeFiles/particles.dir/flags.make

third-party/glfw/examples/CMakeFiles/particles.dir/particles.c.o: third-party/glfw/examples/CMakeFiles/particles.dir/flags.make
third-party/glfw/examples/CMakeFiles/particles.dir/particles.c.o: ../third-party/glfw/examples/particles.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/mica/Desktop/Vulkan/Vulkan-Engine/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object third-party/glfw/examples/CMakeFiles/particles.dir/particles.c.o"
	cd /home/mica/Desktop/Vulkan/Vulkan-Engine/build/third-party/glfw/examples && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/particles.dir/particles.c.o   -c /home/mica/Desktop/Vulkan/Vulkan-Engine/third-party/glfw/examples/particles.c

third-party/glfw/examples/CMakeFiles/particles.dir/particles.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/particles.dir/particles.c.i"
	cd /home/mica/Desktop/Vulkan/Vulkan-Engine/build/third-party/glfw/examples && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/mica/Desktop/Vulkan/Vulkan-Engine/third-party/glfw/examples/particles.c > CMakeFiles/particles.dir/particles.c.i

third-party/glfw/examples/CMakeFiles/particles.dir/particles.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/particles.dir/particles.c.s"
	cd /home/mica/Desktop/Vulkan/Vulkan-Engine/build/third-party/glfw/examples && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/mica/Desktop/Vulkan/Vulkan-Engine/third-party/glfw/examples/particles.c -o CMakeFiles/particles.dir/particles.c.s

third-party/glfw/examples/CMakeFiles/particles.dir/__/deps/tinycthread.c.o: third-party/glfw/examples/CMakeFiles/particles.dir/flags.make
third-party/glfw/examples/CMakeFiles/particles.dir/__/deps/tinycthread.c.o: ../third-party/glfw/deps/tinycthread.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/mica/Desktop/Vulkan/Vulkan-Engine/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object third-party/glfw/examples/CMakeFiles/particles.dir/__/deps/tinycthread.c.o"
	cd /home/mica/Desktop/Vulkan/Vulkan-Engine/build/third-party/glfw/examples && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/particles.dir/__/deps/tinycthread.c.o   -c /home/mica/Desktop/Vulkan/Vulkan-Engine/third-party/glfw/deps/tinycthread.c

third-party/glfw/examples/CMakeFiles/particles.dir/__/deps/tinycthread.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/particles.dir/__/deps/tinycthread.c.i"
	cd /home/mica/Desktop/Vulkan/Vulkan-Engine/build/third-party/glfw/examples && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/mica/Desktop/Vulkan/Vulkan-Engine/third-party/glfw/deps/tinycthread.c > CMakeFiles/particles.dir/__/deps/tinycthread.c.i

third-party/glfw/examples/CMakeFiles/particles.dir/__/deps/tinycthread.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/particles.dir/__/deps/tinycthread.c.s"
	cd /home/mica/Desktop/Vulkan/Vulkan-Engine/build/third-party/glfw/examples && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/mica/Desktop/Vulkan/Vulkan-Engine/third-party/glfw/deps/tinycthread.c -o CMakeFiles/particles.dir/__/deps/tinycthread.c.s

third-party/glfw/examples/CMakeFiles/particles.dir/__/deps/getopt.c.o: third-party/glfw/examples/CMakeFiles/particles.dir/flags.make
third-party/glfw/examples/CMakeFiles/particles.dir/__/deps/getopt.c.o: ../third-party/glfw/deps/getopt.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/mica/Desktop/Vulkan/Vulkan-Engine/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building C object third-party/glfw/examples/CMakeFiles/particles.dir/__/deps/getopt.c.o"
	cd /home/mica/Desktop/Vulkan/Vulkan-Engine/build/third-party/glfw/examples && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/particles.dir/__/deps/getopt.c.o   -c /home/mica/Desktop/Vulkan/Vulkan-Engine/third-party/glfw/deps/getopt.c

third-party/glfw/examples/CMakeFiles/particles.dir/__/deps/getopt.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/particles.dir/__/deps/getopt.c.i"
	cd /home/mica/Desktop/Vulkan/Vulkan-Engine/build/third-party/glfw/examples && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/mica/Desktop/Vulkan/Vulkan-Engine/third-party/glfw/deps/getopt.c > CMakeFiles/particles.dir/__/deps/getopt.c.i

third-party/glfw/examples/CMakeFiles/particles.dir/__/deps/getopt.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/particles.dir/__/deps/getopt.c.s"
	cd /home/mica/Desktop/Vulkan/Vulkan-Engine/build/third-party/glfw/examples && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/mica/Desktop/Vulkan/Vulkan-Engine/third-party/glfw/deps/getopt.c -o CMakeFiles/particles.dir/__/deps/getopt.c.s

# Object files for target particles
particles_OBJECTS = \
"CMakeFiles/particles.dir/particles.c.o" \
"CMakeFiles/particles.dir/__/deps/tinycthread.c.o" \
"CMakeFiles/particles.dir/__/deps/getopt.c.o"

# External object files for target particles
particles_EXTERNAL_OBJECTS =

third-party/glfw/examples/particles: third-party/glfw/examples/CMakeFiles/particles.dir/particles.c.o
third-party/glfw/examples/particles: third-party/glfw/examples/CMakeFiles/particles.dir/__/deps/tinycthread.c.o
third-party/glfw/examples/particles: third-party/glfw/examples/CMakeFiles/particles.dir/__/deps/getopt.c.o
third-party/glfw/examples/particles: third-party/glfw/examples/CMakeFiles/particles.dir/build.make
third-party/glfw/examples/particles: third-party/glfw/src/libglfw3.a
third-party/glfw/examples/particles: /usr/lib/x86_64-linux-gnu/libm.so
third-party/glfw/examples/particles: /usr/lib/x86_64-linux-gnu/librt.so
third-party/glfw/examples/particles: third-party/glfw/examples/CMakeFiles/particles.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/mica/Desktop/Vulkan/Vulkan-Engine/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Linking C executable particles"
	cd /home/mica/Desktop/Vulkan/Vulkan-Engine/build/third-party/glfw/examples && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/particles.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
third-party/glfw/examples/CMakeFiles/particles.dir/build: third-party/glfw/examples/particles

.PHONY : third-party/glfw/examples/CMakeFiles/particles.dir/build

third-party/glfw/examples/CMakeFiles/particles.dir/clean:
	cd /home/mica/Desktop/Vulkan/Vulkan-Engine/build/third-party/glfw/examples && $(CMAKE_COMMAND) -P CMakeFiles/particles.dir/cmake_clean.cmake
.PHONY : third-party/glfw/examples/CMakeFiles/particles.dir/clean

third-party/glfw/examples/CMakeFiles/particles.dir/depend:
	cd /home/mica/Desktop/Vulkan/Vulkan-Engine/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/mica/Desktop/Vulkan/Vulkan-Engine /home/mica/Desktop/Vulkan/Vulkan-Engine/third-party/glfw/examples /home/mica/Desktop/Vulkan/Vulkan-Engine/build /home/mica/Desktop/Vulkan/Vulkan-Engine/build/third-party/glfw/examples /home/mica/Desktop/Vulkan/Vulkan-Engine/build/third-party/glfw/examples/CMakeFiles/particles.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : third-party/glfw/examples/CMakeFiles/particles.dir/depend

