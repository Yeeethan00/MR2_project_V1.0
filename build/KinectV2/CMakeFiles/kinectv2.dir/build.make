# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.5

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
CMAKE_SOURCE_DIR = /home/mypc/Robocon2019/MR2_project_V1.0

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/mypc/Robocon2019/MR2_project_V1.0/build

# Include any dependencies generated for this target.
include KinectV2/CMakeFiles/kinectv2.dir/depend.make

# Include the progress variables for this target.
include KinectV2/CMakeFiles/kinectv2.dir/progress.make

# Include the compile flags for this target's objects.
include KinectV2/CMakeFiles/kinectv2.dir/flags.make

KinectV2/CMakeFiles/kinectv2.dir/KinectV2_func.cpp.o: KinectV2/CMakeFiles/kinectv2.dir/flags.make
KinectV2/CMakeFiles/kinectv2.dir/KinectV2_func.cpp.o: ../KinectV2/KinectV2_func.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/mypc/Robocon2019/MR2_project_V1.0/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object KinectV2/CMakeFiles/kinectv2.dir/KinectV2_func.cpp.o"
	cd /home/mypc/Robocon2019/MR2_project_V1.0/build/KinectV2 && /usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/kinectv2.dir/KinectV2_func.cpp.o -c /home/mypc/Robocon2019/MR2_project_V1.0/KinectV2/KinectV2_func.cpp

KinectV2/CMakeFiles/kinectv2.dir/KinectV2_func.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/kinectv2.dir/KinectV2_func.cpp.i"
	cd /home/mypc/Robocon2019/MR2_project_V1.0/build/KinectV2 && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/mypc/Robocon2019/MR2_project_V1.0/KinectV2/KinectV2_func.cpp > CMakeFiles/kinectv2.dir/KinectV2_func.cpp.i

KinectV2/CMakeFiles/kinectv2.dir/KinectV2_func.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/kinectv2.dir/KinectV2_func.cpp.s"
	cd /home/mypc/Robocon2019/MR2_project_V1.0/build/KinectV2 && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/mypc/Robocon2019/MR2_project_V1.0/KinectV2/KinectV2_func.cpp -o CMakeFiles/kinectv2.dir/KinectV2_func.cpp.s

KinectV2/CMakeFiles/kinectv2.dir/KinectV2_func.cpp.o.requires:

.PHONY : KinectV2/CMakeFiles/kinectv2.dir/KinectV2_func.cpp.o.requires

KinectV2/CMakeFiles/kinectv2.dir/KinectV2_func.cpp.o.provides: KinectV2/CMakeFiles/kinectv2.dir/KinectV2_func.cpp.o.requires
	$(MAKE) -f KinectV2/CMakeFiles/kinectv2.dir/build.make KinectV2/CMakeFiles/kinectv2.dir/KinectV2_func.cpp.o.provides.build
.PHONY : KinectV2/CMakeFiles/kinectv2.dir/KinectV2_func.cpp.o.provides

KinectV2/CMakeFiles/kinectv2.dir/KinectV2_func.cpp.o.provides.build: KinectV2/CMakeFiles/kinectv2.dir/KinectV2_func.cpp.o


KinectV2/CMakeFiles/kinectv2.dir/kinectv2_automoc.cpp.o: KinectV2/CMakeFiles/kinectv2.dir/flags.make
KinectV2/CMakeFiles/kinectv2.dir/kinectv2_automoc.cpp.o: KinectV2/kinectv2_automoc.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/mypc/Robocon2019/MR2_project_V1.0/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object KinectV2/CMakeFiles/kinectv2.dir/kinectv2_automoc.cpp.o"
	cd /home/mypc/Robocon2019/MR2_project_V1.0/build/KinectV2 && /usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/kinectv2.dir/kinectv2_automoc.cpp.o -c /home/mypc/Robocon2019/MR2_project_V1.0/build/KinectV2/kinectv2_automoc.cpp

KinectV2/CMakeFiles/kinectv2.dir/kinectv2_automoc.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/kinectv2.dir/kinectv2_automoc.cpp.i"
	cd /home/mypc/Robocon2019/MR2_project_V1.0/build/KinectV2 && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/mypc/Robocon2019/MR2_project_V1.0/build/KinectV2/kinectv2_automoc.cpp > CMakeFiles/kinectv2.dir/kinectv2_automoc.cpp.i

KinectV2/CMakeFiles/kinectv2.dir/kinectv2_automoc.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/kinectv2.dir/kinectv2_automoc.cpp.s"
	cd /home/mypc/Robocon2019/MR2_project_V1.0/build/KinectV2 && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/mypc/Robocon2019/MR2_project_V1.0/build/KinectV2/kinectv2_automoc.cpp -o CMakeFiles/kinectv2.dir/kinectv2_automoc.cpp.s

KinectV2/CMakeFiles/kinectv2.dir/kinectv2_automoc.cpp.o.requires:

.PHONY : KinectV2/CMakeFiles/kinectv2.dir/kinectv2_automoc.cpp.o.requires

KinectV2/CMakeFiles/kinectv2.dir/kinectv2_automoc.cpp.o.provides: KinectV2/CMakeFiles/kinectv2.dir/kinectv2_automoc.cpp.o.requires
	$(MAKE) -f KinectV2/CMakeFiles/kinectv2.dir/build.make KinectV2/CMakeFiles/kinectv2.dir/kinectv2_automoc.cpp.o.provides.build
.PHONY : KinectV2/CMakeFiles/kinectv2.dir/kinectv2_automoc.cpp.o.provides

KinectV2/CMakeFiles/kinectv2.dir/kinectv2_automoc.cpp.o.provides.build: KinectV2/CMakeFiles/kinectv2.dir/kinectv2_automoc.cpp.o


# Object files for target kinectv2
kinectv2_OBJECTS = \
"CMakeFiles/kinectv2.dir/KinectV2_func.cpp.o" \
"CMakeFiles/kinectv2.dir/kinectv2_automoc.cpp.o"

# External object files for target kinectv2
kinectv2_EXTERNAL_OBJECTS =

KinectV2/libkinectv2.a: KinectV2/CMakeFiles/kinectv2.dir/KinectV2_func.cpp.o
KinectV2/libkinectv2.a: KinectV2/CMakeFiles/kinectv2.dir/kinectv2_automoc.cpp.o
KinectV2/libkinectv2.a: KinectV2/CMakeFiles/kinectv2.dir/build.make
KinectV2/libkinectv2.a: KinectV2/CMakeFiles/kinectv2.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/mypc/Robocon2019/MR2_project_V1.0/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX static library libkinectv2.a"
	cd /home/mypc/Robocon2019/MR2_project_V1.0/build/KinectV2 && $(CMAKE_COMMAND) -P CMakeFiles/kinectv2.dir/cmake_clean_target.cmake
	cd /home/mypc/Robocon2019/MR2_project_V1.0/build/KinectV2 && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/kinectv2.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
KinectV2/CMakeFiles/kinectv2.dir/build: KinectV2/libkinectv2.a

.PHONY : KinectV2/CMakeFiles/kinectv2.dir/build

KinectV2/CMakeFiles/kinectv2.dir/requires: KinectV2/CMakeFiles/kinectv2.dir/KinectV2_func.cpp.o.requires
KinectV2/CMakeFiles/kinectv2.dir/requires: KinectV2/CMakeFiles/kinectv2.dir/kinectv2_automoc.cpp.o.requires

.PHONY : KinectV2/CMakeFiles/kinectv2.dir/requires

KinectV2/CMakeFiles/kinectv2.dir/clean:
	cd /home/mypc/Robocon2019/MR2_project_V1.0/build/KinectV2 && $(CMAKE_COMMAND) -P CMakeFiles/kinectv2.dir/cmake_clean.cmake
.PHONY : KinectV2/CMakeFiles/kinectv2.dir/clean

KinectV2/CMakeFiles/kinectv2.dir/depend:
	cd /home/mypc/Robocon2019/MR2_project_V1.0/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/mypc/Robocon2019/MR2_project_V1.0 /home/mypc/Robocon2019/MR2_project_V1.0/KinectV2 /home/mypc/Robocon2019/MR2_project_V1.0/build /home/mypc/Robocon2019/MR2_project_V1.0/build/KinectV2 /home/mypc/Robocon2019/MR2_project_V1.0/build/KinectV2/CMakeFiles/kinectv2.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : KinectV2/CMakeFiles/kinectv2.dir/depend
