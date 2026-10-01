# SPDX-License-Identifier: GPL-3.0-or-later
cmake_minimum_required(VERSION 3.16)
project(facebuilder VERSION 0.2.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Svg)

add_executable(facebuilder
  src/main.cpp
  src/mainwindow.cpp
  src/mainwindow.h
  src/facescene.cpp
  src/facescene.h
  src/face.cpp
  src/face.h
  src/facepart.cpp
  src/facepart.h
  src/facepartitem.cpp
  src/facepartitem.h
  src/partbrowser.cpp
  src/partbrowser.h
  src/parttypes.h
  src/commands.cpp
  src/commands.h
  src/xmlfaceformat.cpp
  src/xmlfaceformat.h
  src/faceexport.cpp
  src/faceexport.h
  src/paths.cpp
  src/paths.h
)

target_include_directories(facebuilder PRIVATE src)

target_link_libraries(facebuilder PRIVATE
  Qt6::Widgets
  Qt6::Svg
)

include(GNUInstallDirs)

install(TARGETS facebuilder RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(DIRECTORY data/ DESTINATION ${CMAKE_INSTALL_DATADIR}/facebuilder/data)
install(DIRECTORY resources/icons/ DESTINATION ${CMAKE_INSTALL_DATADIR}/facebuilder/icons)
# Example faces for Open dialog convenience (optional but useful)
install(DIRECTORY examples/ DESTINATION ${CMAKE_INSTALL_DATADIR}/facebuilder/examples)

install(FILES resources/facebuilder.desktop
        DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)
install(DIRECTORY resources/icons/hicolor/
        DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor)
install(FILES resources/icons/facebuilder.png
        DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/256x256/apps
        RENAME facebuilder.png)
