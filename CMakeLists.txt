#
# Cozmonaut
# Copyright 2019 The Cozmonaut Contributors
#

cmake_minimum_required(VERSION 3.4)
project(Cozmonaut)

add_subdirectory(third_party/fmt)
add_subdirectory(third_party/glad)
add_subdirectory(third_party/glfw)
add_subdirectory(third_party/nanovg-cmake)

find_package(Curses REQUIRED)
find_package(PythonInterp 3.6 REQUIRED)
find_package(PythonLibs 3.6 REQUIRED)
find_package(Threads REQUIRED)

set(cozmo_SRC_FILES
        src/service/console.c
        src/service/monitor.c
        src/service/python.c
        src/global.c
        src/log.c
        src/log_fmt.cpp
        src/main.c
        src/service.c
        )

add_executable(cozmo ${cozmo_SRC_FILES})
set_target_properties(cozmo PROPERTIES C_STANDARD 99 CXX_STANDARD 14)
target_include_directories(cozmo PRIVATE ${CURSES_INCLUDE_DIR} ${PYTHON_INCLUDE_DIRS})
target_link_libraries(cozmo PRIVATE fmt::fmt-header-only glad glfw nanovg ${CURSES_LIBRARY} ${PYTHON_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
