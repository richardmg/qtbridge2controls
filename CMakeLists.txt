cmake_minimum_required(VERSION 3.16)

project(application VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 6.5 REQUIRED Gui Quick QuickControls2)

qt_add_executable(application
    main.cpp
)

qt_add_resources(application "resources"
    PREFIX "/"
    FILES
        main.qml
)

target_link_libraries(application
    PRIVATE
        Qt6::Quick
        Qt6::QuickControls2
        Qt6::GuiPrivate
)

qt_add_qml_module(application
    URI application
    VERSION 1.0
    QML_FILES main.qml 
)

set_target_properties(application PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER application.me.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)
