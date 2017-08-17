cmake_minimum_required(VERSION 2.8)
project(AntiCheat)

SET( OpenCV_DIR "/usr/local/Cellar/opencv3/3.2.0/share/OpenCV/")
find_package( OpenCV REQUIRED )
set(CMAKE_BUILD_TYPE RELEASE)
include_directories(include
    ${OpenCV_INCLUDE_DIRS})
add_subdirectory(include)
add_subdirectory(src)




