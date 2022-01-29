#
# Cozmonaut
# Copyright 2019 The Cozmonaut Contributors
#

cmake_minimum_required(VERSION 3.4)
project(Cozmonaut)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

find_package(CK REQUIRED)

set(cozmo_SRC_FILES
        src/global.c
        src/log.c
        src/main.c
        src/service.c
        )

add_executable(cozmo ${cozmo_SRC_FILES})
set_target_properties(cozmo PROPERTIES C_STANDARD 99)
target_include_directories(cozmo PRIVATE ${CK_INCLUDE_DIRS})
target_link_libraries(cozmo PRIVATE ${CK_LIBRARIES})
