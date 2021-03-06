cmake_minimum_required(VERSION 3.5.1)
project(aoc2019_11 VERSION 0.0.1)

# for correct library locations across platforms
include(GNUInstallDirs)
find_package(Threads REQUIRED)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# BUILD

include_directories(SYSTEM
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
      test/test_intcode.cpp
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
