cmake_minimum_required(VERSION 3.5.1)
project(aoc2019_25 VERSION 0.0.1)

# for correct library locations across platforms
include(GNUInstallDirs)

find_package(Threads REQUIRED)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE "RelWithDebInfo")

# BUILD

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  message(STATUS "Building with debug output.")
  add_definitions(-DDEBUG)
endif()

add_compile_options(
  -Wall
  #-Wextra
  #-Werror
  #-Wpedantic
)

include_directories(
  include
)

add_library(
  ${PROJECT_NAME}_lib
  SHARED
  src/challenge_lib.c
  src/intcode.c
)

add_executable(
  ${PROJECT_NAME}
  src/main.c
)

target_link_libraries(${PROJECT_NAME}
  ${PROJECT_NAME}_lib
  ${CMAKE_THREAD_LIBS_INIT}
)
