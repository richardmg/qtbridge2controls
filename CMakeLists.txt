cmake_minimum_required(VERSION 3.16)

project(qtbridge2controls VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 6.5 REQUIRED Gui Quick QuickControls2)

qt_add_executable(qtbridge2controls
    src/main.cpp
    src/qtbridgereader.h
    src/jsontools.h src/jsontools.cpp
    src/stylegenerator.h src/stylegenerator.cpp
)

qt_add_resources(qtbridge2controls "controls"
    PREFIX "/"
    BASE templates/controls
    FILES
        templates/controls/Button.qml
)

target_link_libraries(qtbridge2controls
    PRIVATE
        Qt6::GuiPrivate
)
