cmake_minimum_required(VERSION 3.16)

project(crash_report VERSION 1.0.0 LANGUAGES CXX)

add_subdirectory(lib/zlib)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

qt_standard_project_setup()

qt_add_executable(crash_report
        src/main.ui
        src/main.cpp
)

target_link_libraries(crash_report PRIVATE Qt6::Widgets zlib)

set_target_properties(crash_report PROPERTIES
        WIN32_EXECUTABLE ON
        MACOSX_BUNDLE ON
)