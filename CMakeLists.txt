project(MR2_project_V1)

cmake_minimum_required(VERSION 2.8.11 FATAL_ERROR) 
 
set(CMAKE_PREFIX_PATH /opt/Qt5.9.1/5.9.1/gcc_64/lib/cmake) 

set(CMAKE_INCLUDE_CURRENT_DIR ON) 
set(CMAKE_AUTOMOC ON) 
set(CMAKE_AUTOUIC ON) 
set(CMAKE_AUTORCC ON)
set(SRCS main.cpp)

find_package(Qt5Charts REQUIRED)
FIND_PACKAGE(Qt5Widgets REQUIRED)
FIND_PACKAGE(Qt5Gui REQUIRED)
FIND_PACKAGE(Qt5Core REQUIRED)
FIND_PACKAGE(Qt5OpenGL REQUIRED)
qt5_wrap_cpp( MOC mainwindow.h) 
qt5_wrap_ui( UIC mainwindow.ui)

#set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} $ENV{HOME}/freenect2/lib/cmake/freenect2 ) 
#find_package(freenect2 REQUIRED) 
#find_package(PCL 1.7 REQUIRED) 
#find_package(OpenCV REQUIRED) 
#find_package(Eigen3 REQUIRED)

#include_directories("/usr/include/libusb-1.0/") 
#include_directories( ${freenect2_INCLUDE_DIR} ${PCL_INCLUDE_DIRS} ${OpenCV_INCLUDE_DIRS}) 

#link_directories( ${PCL_LIBRARY_DIRS} ) 

#add_definitions( ${PCL_DEFINITIONS} )

add_subdirectory(Users)
add_subdirectory(Hokuyo_urg)
add_subdirectory(KinectV2)
add_subdirectory(Limbs)
add_subdirectory(Can)
add_subdirectory(Utils)
add_subdirectory(GUI)

add_executable(MR2_project_V1 ${SRCS} ./GUI/gui_main.cpp ./GUI/gui_main.h ./GUI/mainwindow.cpp ./GUI/mainwindow.h ./GUI/mainwindow.ui
               ./GUI/cradar.cpp ./GUI/cradar.h ${RESOURCE_DIR})

target_link_libraries(MR2_project_V1 #${freenect2_LIBRARIES} ${OpenCV_LIBS}
				     users hokuyo_urg #kinectv2 
                     limbs pthread can utils #gui
				     Qt5::Widgets  Qt5::Charts  Qt5::Gui  Qt5::Core  Qt5::OpenGL)

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
    message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

