cmake_minimum_required(VERSION 3.12)

project(l6_z2)

set(CMAKE_CXX_STANDARD 20)

find_package(Qt6 COMPONENTS Core Widgets Network REQUIRED)

qt_standard_project_setup()

qt_add_executable(l6_z2

    mywidget.ui
    mywidget.cpp
    mywidget.h
    main.cpp
)

target_link_libraries(l6_z2 PRIVATE Qt6::Widgets Qt6::Network)
