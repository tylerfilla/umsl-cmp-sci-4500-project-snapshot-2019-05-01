#
# Cozmonaut
# Copyright 2019 The Cozmonaut Contributors
#

cmake_minimum_required(VERSION 3.4)
project(Cozmonaut)

add_subdirectory(third_party/fmt)

set(cozmo_SRC_FILES
        src/global.c
        src/log.c
        src/log_fmt.cpp
        src/main.c
        )

add_executable(cozmo ${cozmo_SRC_FILES})
set_target_properties(cozmo PROPERTIES C_STANDARD 99 CXX_STANDARD 14)
target_link_libraries(cozmo PRIVATE fmt::fmt-header-only)
