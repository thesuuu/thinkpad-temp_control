cmake_minimum_required(VERSION 3.5)

project(fancontrol)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_AUTOMOC ON)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

add_executable(fancontrol
    main.cpp
    mainwindow.cpp
    mainwindow.h
)

target_link_libraries(fancontrol Qt5::Widgets)