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
include third-party/KTX-Software/tests/gtest/CMakeFiles/gtest.dir/depend.make

# Include the progress variables for this target.
include third-party/KTX-Software/tests/gtest/CMakeFiles/gtest.dir/progress.make

# Include the compile flags for this target's objects.
include third-party/KTX-Software/tests/gtest/CMakeFiles/gtest.dir/flags.make

third-party/KTX-Software/tests/gtest/CMakeFiles/gtest.dir/src/gtest-all.cc.o: third-party/KTX-Software/tests/gtest/CMakeFiles/gtest.dir/flags.make
third-party/KTX-Software/tests/gtest/CMakeFiles/gtest.dir/src/gtest-all.cc.o: ../third-party/KTX-Software/tests/gtest/src/gtest-all.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/mica/Desktop/Vulkan/Vulkan-Engine/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object third-party/KTX-Software/tests/gtest/CMakeFiles/gtest.dir/src/gtest-all.cc.o"
	cd /home/mica/Desktop/Vulkan/Vulkan-Engine/build/third-party/KTX-Software/tests/gtest && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -Wno-missing-field-initializers -o CMakeFiles/gtest.dir/src/gtest-all.cc.o -c /home/mica/Desktop/Vulkan/Vulkan-Engine/third-party/KTX-Software/tests/gtest/src/gtest-all.cc

third-party/KTX-Software/tests/gtest/CMakeFiles/gtest.dir/src/gtest-all.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/gtest.dir/src/gtest-all.cc.i"
	cd /home/mica/Desktop/Vulkan/Vulkan-Engine/build/third-party/KTX-Software/tests/gtest && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -Wno-missing-field-initializers -E /home/mica/Desktop/Vulkan/Vulkan-Engine/third-party/KTX-Software/tests/gtest/src/gtest-all.cc > CMakeFiles/gtest.dir/src/gtest-all.cc.i

third-party/KTX-Software/tests/gtest/CMakeFiles/gtest.dir/src/gtest-all.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/gtest.dir/src/gtest-all.cc.s"
	cd /home/mica/Desktop/Vulkan/Vulkan-Engine/build/third-party/KTX-Software/tests/gtest && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -Wno-missing-field-initializers -S /home/mica/Desktop/Vulkan/Vulkan-Engine/third-party/KTX-Software/tests/gtest/src/gtest-all.cc -o CMakeFiles/gtest.dir/src/gtest-all.cc.s

third-party/KTX-Software/tests/gtest/CMakeFiles/gtest.dir/src/gtest_main.cc.o: third-party/KTX-Software/tests/gtest/CMakeFiles/gtest.dir/flags.make
third-party/KTX-Software/tests/gtest/CMakeFiles/gtest.dir/src/gtest_main.cc.o: ../third-party/KTX-Software/tests/gtest/src/gtest_main.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/mica/Desktop/Vulkan/Vulkan-Engine/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object third-party/KTX-Software/tests/gtest/CMakeFiles/gtest.dir/src/gtest_main.cc.o"
	cd /home/mica/Desktop/Vulkan/Vulkan-Engine/build/third-party/KTX-Software/tests/gtest && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/gtest.dir/src/gtest_main.cc.o -c /home/mica/Desktop/Vulkan/Vulkan-Engine/third-party/KTX-Software/tests/gtest/src/gtest_main.cc

third-party/KTX-Software/tests/gtest/CMakeFiles/gtest.dir/src/gtest_main.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/gtest.dir/src/gtest_main.cc.i"
	cd /home/mica/Desktop/Vulkan/Vulkan-Engine/build/third-party/KTX-Software/tests/gtest && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/mica/Desktop/Vulkan/Vulkan-Engine/third-party/KTX-Software/tests/gtest/src/gtest_main.cc > CMakeFiles/gtest.dir/src/gtest_main.cc.i

third-party/KTX-Software/tests/gtest/CMakeFiles/gtest.dir/src/gtest_main.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/gtest.dir/src/gtest_main.cc.s"
	cd /home/mica/Desktop/Vulkan/Vulkan-Engine/build/third-party/KTX-Software/tests/gtest && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/mica/Desktop/Vulkan/Vulkan-Engine/third-party/KTX-Software/tests/gtest/src/gtest_main.cc -o CMakeFiles/gtest.dir/src/gtest_main.cc.s

# Object files for target gtest
gtest_OBJECTS = \
"CMakeFiles/gtest.dir/src/gtest-all.cc.o" \
"CMakeFiles/gtest.dir/src/gtest_main.cc.o"

# External object files for target gtest
gtest_EXTERNAL_OBJECTS =

third-party/KTX-Software/tests/gtest/libgtest.a: third-party/KTX-Software/tests/gtest/CMakeFiles/gtest.dir/src/gtest-all.cc.o
third-party/KTX-Software/tests/gtest/libgtest.a: third-party/KTX-Software/tests/gtest/CMakeFiles/gtest.dir/src/gtest_main.cc.o
third-party/KTX-Software/tests/gtest/libgtest.a: third-party/KTX-Software/tests/gtest/CMakeFiles/gtest.dir/build.make
third-party/KTX-Software/tests/gtest/libgtest.a: third-party/KTX-Software/tests/gtest/CMakeFiles/gtest.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/mica/Desktop/Vulkan/Vulkan-Engine/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX static library libgtest.a"
	cd /home/mica/Desktop/Vulkan/Vulkan-Engine/build/third-party/KTX-Software/tests/gtest && $(CMAKE_COMMAND) -P CMakeFiles/gtest.dir/cmake_clean_target.cmake
	cd /home/mica/Desktop/Vulkan/Vulkan-Engine/build/third-party/KTX-Software/tests/gtest && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/gtest.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
third-party/KTX-Software/tests/gtest/CMakeFiles/gtest.dir/build: third-party/KTX-Software/tests/gtest/libgtest.a

.PHONY : third-party/KTX-Software/tests/gtest/CMakeFiles/gtest.dir/build

third-party/KTX-Software/tests/gtest/CMakeFiles/gtest.dir/clean:
	cd /home/mica/Desktop/Vulkan/Vulkan-Engine/build/third-party/KTX-Software/tests/gtest && $(CMAKE_COMMAND) -P CMakeFiles/gtest.dir/cmake_clean.cmake
.PHONY : third-party/KTX-Software/tests/gtest/CMakeFiles/gtest.dir/clean

third-party/KTX-Software/tests/gtest/CMakeFiles/gtest.dir/depend:
	cd /home/mica/Desktop/Vulkan/Vulkan-Engine/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/mica/Desktop/Vulkan/Vulkan-Engine /home/mica/Desktop/Vulkan/Vulkan-Engine/third-party/KTX-Software/tests/gtest /home/mica/Desktop/Vulkan/Vulkan-Engine/build /home/mica/Desktop/Vulkan/Vulkan-Engine/build/third-party/KTX-Software/tests/gtest /home/mica/Desktop/Vulkan/Vulkan-Engine/build/third-party/KTX-Software/tests/gtest/CMakeFiles/gtest.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : third-party/KTX-Software/tests/gtest/CMakeFiles/gtest.dir/depend

