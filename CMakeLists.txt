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
)

target_link_libraries(facebuilder PRIVATE
  Qt6::Widgets
  Qt6::Svg
)

# Install data next to the binary for development runs; later packaging can relocate.
# For now we rely on running from the build tree or setting FACEBUILDER_DATA.
install(TARGETS facebuilder RUNTIME DESTINATION bin)
install(DIRECTORY data/ DESTINATION share/facebuilder/data)
