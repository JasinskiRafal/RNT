cmake_minimum_required(VERSION 3.0)

project(RapidNoteTaker)

find_package(OpenCV REQUIRED)


add_library(RapidNoteTaker src/image.cpp)
target_include_directories(RapidNoteTaker PUBLIC inc)
target_link_libraries(RapidNoteTaker PUBLIC ${OpenCV_LIBS})

add_executable(RapidNoteTakerMain main.cpp)
target_link_libraries(RapidNoteTakerMain PRIVATE RapidNoteTaker)
