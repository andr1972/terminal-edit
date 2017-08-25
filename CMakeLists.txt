cmake_minimum_required(VERSION 3.5)
project(terminal-edit)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

set(SOURCE_FILES
    src/ColumnPrinter.cpp
    src/History.cpp
    src/strutils.cpp
    src/Terminal.cpp
    src/TermLines.cpp
    example/CommandProcessor.cpp
    example/FileTab.cpp
    example/main.cpp)

add_executable(terminal-edit ${SOURCE_FILES})

target_link_libraries(terminal-edit curses boost_filesystem boost_system)