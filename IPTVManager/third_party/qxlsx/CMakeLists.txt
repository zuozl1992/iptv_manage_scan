# QXlsx placeholder CMakeLists.txt
# Replace this with actual QXlsx source code from https://github.com/QtExcel/QXlsx

cmake_minimum_required(VERSION 3.19)

project(QXlsx LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui)

add_library(QXlsx STATIC
    # Add QXlsx source files here
    # QXlsx/xlsxdocument.cpp
    # QXlsx/xlsxformat.cpp
    # etc.
)

target_include_directories(QXlsx PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(QXlsx PUBLIC Qt::Core Qt::Gui)

# Placeholder - no actual implementation
# Download QXlsx from https://github.com/QtExcel/QXlsx
message(WARNING "QXlsx is a placeholder. Download from https://github.com/QtExcel/QXlsx")
