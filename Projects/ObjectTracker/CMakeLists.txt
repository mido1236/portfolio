cmake_minimum_required(VERSION 3.20)
project(ObjectTracker)

set(CMAKE_CXX_STANDARD 17)

# OpenCV Include Directory
include_directories("C:/opencv/build/include")

# OpenCV Library Directories
link_directories("C:/opencv/build/lib/Release")

# Define the Executable
add_executable(ObjectTracker main.cpp)

# Link OpenCV Libraries
target_link_libraries(ObjectTracker
        opencv_core4100
        opencv_imgproc4100
        opencv_highgui4100
        opencv_videoio4100
)
