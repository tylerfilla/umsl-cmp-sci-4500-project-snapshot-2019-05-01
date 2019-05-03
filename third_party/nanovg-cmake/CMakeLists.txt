#
# Cozmonaut
# Copyright 2019 The Cozmonaut Contributors
#

cmake_minimum_required(VERSION 3.4)
project(nanovg)

set(nanovg_SRC_FILES
        src/nanovg.c
        )

add_library(nanovg STATIC ${nanovg_SRC_FILES})
target_include_directories(nanovg PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../nanovg/src")
target_include_directories(nanovg PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/")
