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
include third-party/KTX-Software/tests/streamtests/CMakeFiles/streamtests.dir/depend.make

# Include the progress variables for this target.
include third-party/KTX-Software/tests/streamtests/CMakeFiles/streamtests.dir/progress.make

# Include the compile flags for this target's objects.
include third-party/KTX-Software/tests/streamtests/CMakeFiles/streamtests.dir/flags.make

third-party/KTX-Software/tests/streamtests/CMakeFiles/streamtests.dir/streamtests.cc.o: third-party/KTX-Software/tests/streamtests/CMakeFiles/streamtests.dir/flags.make
third-party/KTX-Software/tests/streamtests/CMakeFiles/streamtests.dir/streamtests.cc.o: ../third-party/KTX-Software/tests/streamtests/streamtests.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/mica/Desktop/Vulkan/Vulkan-Engine/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object third-party/KTX-Software/tests/streamtests/CMakeFiles/streamtests.dir/streamtests.cc.o"
	cd /home/mica/Desktop/Vulkan/Vulkan-Engine/build/third-party/KTX-Software/tests/streamtests && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/streamtests.dir/streamtests.cc.o -c /home/mica/Desktop/Vulkan/Vulkan-Engine/third-party/KTX-Software/tests/streamtests/streamtests.cc

third-party/KTX-Software/tests/streamtests/CMakeFiles/streamtests.dir/streamtests.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/streamtests.dir/streamtests.cc.i"
	cd /home/mica/Desktop/Vulkan/Vulkan-Engine/build/third-party/KTX-Software/tests/streamtests && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/mica/Desktop/Vulkan/Vulkan-Engine/third-party/KTX-Software/tests/streamtests/streamtests.cc > CMakeFiles/streamtests.dir/streamtests.cc.i

third-party/KTX-Software/tests/streamtests/CMakeFiles/streamtests.dir/streamtests.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/streamtests.dir/streamtests.cc.s"
	cd /home/mica/Desktop/Vulkan/Vulkan-Engine/build/third-party/KTX-Software/tests/streamtests && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/mica/Desktop/Vulkan/Vulkan-Engine/third-party/KTX-Software/tests/streamtests/streamtests.cc -o CMakeFiles/streamtests.dir/streamtests.cc.s

# Object files for target streamtests
streamtests_OBJECTS = \
"CMakeFiles/streamtests.dir/streamtests.cc.o"

# External object files for target streamtests
streamtests_EXTERNAL_OBJECTS =

third-party/KTX-Software/tests/streamtests/streamtests: third-party/KTX-Software/tests/streamtests/CMakeFiles/streamtests.dir/streamtests.cc.o
third-party/KTX-Software/tests/streamtests/streamtests: third-party/KTX-Software/tests/streamtests/CMakeFiles/streamtests.dir/build.make
third-party/KTX-Software/tests/streamtests/streamtests: third-party/KTX-Software/tests/gtest/libgtest.a
third-party/KTX-Software/tests/streamtests/streamtests: third-party/KTX-Software/libktx.so.4.1.0
third-party/KTX-Software/tests/streamtests/streamtests: third-party/KTX-Software/tests/streamtests/CMakeFiles/streamtests.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/mica/Desktop/Vulkan/Vulkan-Engine/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable streamtests"
	cd /home/mica/Desktop/Vulkan/Vulkan-Engine/build/third-party/KTX-Software/tests/streamtests && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/streamtests.dir/link.txt --verbose=$(VERBOSE)
	cd /home/mica/Desktop/Vulkan/Vulkan-Engine/build/third-party/KTX-Software/tests/streamtests && /usr/bin/cmake -D TEST_TARGET=streamtests -D TEST_EXECUTABLE=/home/mica/Desktop/Vulkan/Vulkan-Engine/build/third-party/KTX-Software/tests/streamtests/streamtests -D TEST_EXECUTOR= -D TEST_WORKING_DIR=/home/mica/Desktop/Vulkan/Vulkan-Engine/build/third-party/KTX-Software/tests/streamtests -D TEST_EXTRA_ARGS=/home/mica/Desktop/Vulkan/Vulkan-Engine/third-party/KTX-Software/tests/testimages/ -D TEST_PROPERTIES= -D TEST_PREFIX=streamtest -D TEST_SUFFIX= -D NO_PRETTY_TYPES=FALSE -D NO_PRETTY_VALUES=FALSE -D TEST_LIST=streamtests_TESTS -D CTEST_FILE=/home/mica/Desktop/Vulkan/Vulkan-Engine/build/third-party/KTX-Software/tests/streamtests/streamtests[1]_tests.cmake -D TEST_DISCOVERY_TIMEOUT=5 -P /usr/share/cmake-3.16/Modules/GoogleTestAddTests.cmake

# Rule to build all files generated by this target.
third-party/KTX-Software/tests/streamtests/CMakeFiles/streamtests.dir/build: third-party/KTX-Software/tests/streamtests/streamtests

.PHONY : third-party/KTX-Software/tests/streamtests/CMakeFiles/streamtests.dir/build

third-party/KTX-Software/tests/streamtests/CMakeFiles/streamtests.dir/clean:
	cd /home/mica/Desktop/Vulkan/Vulkan-Engine/build/third-party/KTX-Software/tests/streamtests && $(CMAKE_COMMAND) -P CMakeFiles/streamtests.dir/cmake_clean.cmake
.PHONY : third-party/KTX-Software/tests/streamtests/CMakeFiles/streamtests.dir/clean

third-party/KTX-Software/tests/streamtests/CMakeFiles/streamtests.dir/depend:
	cd /home/mica/Desktop/Vulkan/Vulkan-Engine/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/mica/Desktop/Vulkan/Vulkan-Engine /home/mica/Desktop/Vulkan/Vulkan-Engine/third-party/KTX-Software/tests/streamtests /home/mica/Desktop/Vulkan/Vulkan-Engine/build /home/mica/Desktop/Vulkan/Vulkan-Engine/build/third-party/KTX-Software/tests/streamtests /home/mica/Desktop/Vulkan/Vulkan-Engine/build/third-party/KTX-Software/tests/streamtests/CMakeFiles/streamtests.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : third-party/KTX-Software/tests/streamtests/CMakeFiles/streamtests.dir/depend
