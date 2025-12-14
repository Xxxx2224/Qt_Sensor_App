cmake_minimum_required(VERSION 3.16)

project(YeniSensor VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)


set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Quick Network Core Charts Widgets)

qt_standard_project_setup(REQUIRES 6.5)

qt_add_executable(appYeniSensor
    src/main.cpp
    src/authmanager.cpp
    src/authmanager.h
)

qt_add_qml_module(appYeniSensor
    URI YeniSensor
    VERSION 1.0
    QML_FILES
        ui/Main.qml
        ui/LoginScreen.qml
        ui/HomeScreen.qml
        QML_FILES ui/RegisterScreen.qml
)

target_link_libraries(appYeniSensor
    PRIVATE Qt6::Quick
    PRIVATE Qt6::Network
    PRIVATE Qt6::Core
    PRIVATE Qt6::Charts
    PRIVATE Qt6::Widgets
)

include(GNUInstallDirs)
install(TARGETS appYeniSensor
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
