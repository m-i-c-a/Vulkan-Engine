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

# Utility rule file for toktx_version.

# Include the progress variables for this target.
include third-party/KTX-Software/tools/toktx/CMakeFiles/toktx_version.dir/progress.make

third-party/KTX-Software/tools/toktx/CMakeFiles/toktx_version: ../third-party/KTX-Software/tools/toktx/version.h


../third-party/KTX-Software/tools/toktx/version.h:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/mica/Desktop/Vulkan/Vulkan-Engine/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Generate /home/mica/Desktop/Vulkan/Vulkan-Engine/third-party/KTX-Software/tools/toktx/version.h"
	cd /home/mica/Desktop/Vulkan/Vulkan-Engine/third-party/KTX-Software && ./mkversion -o version.h tools/toktx

toktx_version: third-party/KTX-Software/tools/toktx/CMakeFiles/toktx_version
toktx_version: ../third-party/KTX-Software/tools/toktx/version.h
toktx_version: third-party/KTX-Software/tools/toktx/CMakeFiles/toktx_version.dir/build.make

.PHONY : toktx_version

# Rule to build all files generated by this target.
third-party/KTX-Software/tools/toktx/CMakeFiles/toktx_version.dir/build: toktx_version

.PHONY : third-party/KTX-Software/tools/toktx/CMakeFiles/toktx_version.dir/build

third-party/KTX-Software/tools/toktx/CMakeFiles/toktx_version.dir/clean:
	cd /home/mica/Desktop/Vulkan/Vulkan-Engine/build/third-party/KTX-Software/tools/toktx && $(CMAKE_COMMAND) -P CMakeFiles/toktx_version.dir/cmake_clean.cmake
.PHONY : third-party/KTX-Software/tools/toktx/CMakeFiles/toktx_version.dir/clean

third-party/KTX-Software/tools/toktx/CMakeFiles/toktx_version.dir/depend:
	cd /home/mica/Desktop/Vulkan/Vulkan-Engine/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/mica/Desktop/Vulkan/Vulkan-Engine /home/mica/Desktop/Vulkan/Vulkan-Engine/third-party/KTX-Software/tools/toktx /home/mica/Desktop/Vulkan/Vulkan-Engine/build /home/mica/Desktop/Vulkan/Vulkan-Engine/build/third-party/KTX-Software/tools/toktx /home/mica/Desktop/Vulkan/Vulkan-Engine/build/third-party/KTX-Software/tools/toktx/CMakeFiles/toktx_version.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : third-party/KTX-Software/tools/toktx/CMakeFiles/toktx_version.dir/depend

