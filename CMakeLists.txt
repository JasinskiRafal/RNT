cmake_minimum_required(VERSION 3.0)

project(RapidNoteTaker)

find_package(OpenCV REQUIRED)

find_package(Tesseract)
if (NOT Tesseract_FOUND)
    find_package(PkgConfig REQUIRED)

    pkg_check_modules(Tesseract REQUIRED IMPORTED_TARGET tesseract lept)
    add_library(Tesseract::libtesseract ALIAS PkgConfig::Tesseract)
endif ()

add_library(RapidNoteTaker src/image.cpp src/text.cpp)
target_include_directories(RapidNoteTaker PUBLIC inc)
target_link_libraries(RapidNoteTaker PUBLIC ${OpenCV_LIBS})
target_link_libraries(RapidNoteTaker PRIVATE Tesseract::libtesseract)

add_executable(RapidNoteTakerMain main.cpp)
target_link_libraries(RapidNoteTakerMain PRIVATE RapidNoteTaker)
