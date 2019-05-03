#
# Cozmonaut
# Copyright 2019 The Cozmonaut Contributors
#

cmake_minimum_required(VERSION 3.4)
project(glad)

set(glad_SRC_FILES
        src/glad.c
        )

add_library(glad STATIC ${glad_SRC_FILES})
target_include_directories(glad PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/")
