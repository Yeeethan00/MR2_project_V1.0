project(MR2_project_V1)

cmake_minimum_required(VERSION 2.8)
 
# Set cmake prefix path to enable cmake to find freenect2 
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} $ENV{HOME}/freenect2/lib/cmake/freenect2) 
find_package(freenect2 REQUIRED) 
find_package(PCL 1.7 REQUIRED) 
find_package(OpenCV REQUIRED) 
find_package(Eigen3 REQUIRED)

include_directories("/usr/include/libusb-1.0/") 
INCLUDE_DIRECTORIES( ${freenect2_INCLUDE_DIR} ${PCL_INCLUDE_DIRS} ${OpenCV_INCLUDE_DIRS}) 
link_directories( ${PCL_LIBRARY_DIRS} ) 
add_definitions( ${PCL_DEFINITIONS} )

add_subdirectory(Hokuyo_urg)
add_subdirectory(KinectV2)
add_subdirectory(Limbs)
add_subdirectory(Can)
add_subdirectory(utils)

set(SRCS main.cpp)

add_executable(MR2_project_V1 ${SRCS})

target_link_libraries(MR2_project_V1 hokuyo_urg kinectv2 limbs ${freenect2_LIBRARIES} ${OpenCV_LIBS} pthread can utils)



