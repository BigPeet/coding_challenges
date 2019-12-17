cmake_minimum_required(VERSION 3.5.1)
project(challenge_XXX VERSION 0.0.1)

# for correct library locations across platforms
include(GNUInstallDirs)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE "RelWithDebInfo")

# BUILD

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  message(STATUS "Building with debug output.")
  add_definitions(-DDEBUG)
endif()

include_directories(SYSTEM
  include
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

target_include_directories(
  ${PROJECT_NAME}
  PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
  )

target_compile_options(
  ${PROJECT_NAME}
  PRIVATE
  -Wall
  #-Wextra
  #-Werror
  #-Wpedantic
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
