cmake_minimum_required(VERSION 3.16)

project(reporter VERSION 1.0.0 LANGUAGES CXX)

add_subdirectory(lib/zlib)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

qt_standard_project_setup()

qt_add_executable(reporter
        src/main.ui
        src/main.cpp
)

target_link_libraries(reporter PRIVATE Qt6::Widgets zlib)

set_target_properties(reporter PROPERTIES
        WIN32_EXECUTABLE ON
        MACOSX_BUNDLE ON
)