cmake_minimum_required(VERSION 3.5.1)
project(aoc2019_24 VERSION 0.0.1)

# for correct library locations across platforms
include(GNUInstallDirs)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE "RelWithDebInfo")

# BUILD

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  message(STATUS "Building with debug output.")
  add_definitions(-DDEBUG)
endif()

include_directories(
  include
)

add_compile_options(
  -Wall
  #-Wextra
  #-Werror
  #-Wpedantic
)

add_library(
  ${PROJECT_NAME}_lib
  SHARED
  src/challenge_lib.c
)

add_executable(
  ${PROJECT_NAME}
  src/main.c
)

target_link_libraries(${PROJECT_NAME}
  ${PROJECT_NAME}_lib
)

# Testing

if (BUILD_TESTING)
  find_package(GTest QUIET)
  # only build when modern target exists
  if (TARGET GTest::GTest)
    add_executable(
      ${PROJECT_NAME}-test
      test/test_main.cpp
      test/test_challenge.cpp
      )
    set_property(TARGET ${PROJECT_NAME}-test PROPERTY CXX_STANDARD 11)
    set_property(TARGET ${PROJECT_NAME}-test PROPERTY CXX_STANDARD_REQUIRED ON)
    target_link_libraries(
      ${PROJECT_NAME}-test
      PRIVATE
      ${PROJECT_NAME}_lib
      GTest::GTest
      GTest::Main
      )

    add_test(
      ${PROJECT_NAME}-test
      ${PROJECT_NAME}-test
      )

    install(
      TARGETS ${PROJECT_NAME}-test
      RUNTIME DESTINATION build
      )
  endif()
endif()
