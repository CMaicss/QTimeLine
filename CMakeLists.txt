cmake_minimum_required(VERSION 3.31)
project(qtimeline)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


find_package(Qt6 COMPONENTS
        Core
        Widgets
        REQUIRED)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})


add_executable(qtimeline main.cpp
        src/QTimeAxis.cpp
        src/QTimeAxis.h
        example/GanttChart.cpp
        example/GanttChart.h)

target_link_libraries(qtimeline
        Qt::Core
        Qt::Widgets
)
