cmake_minimum_required(VERSION 3.28)

project(zappy)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if(NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})
endif()

if(EXISTS "${CMAKE_SOURCE_DIR}/.git")
  find_package(Git QUIET)
  if(GIT_FOUND)
    execute_process(
      COMMAND ${GIT_EXECUTABLE} config core.hooksPath .githooks
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
      RESULT_VARIABLE GIT_HOOKS_RESULT
      OUTPUT_QUIET ERROR_QUIET)
    if(GIT_HOOKS_RESULT EQUAL 0)
      message(STATUS "Git hooks activated (core.hooksPath=.githooks)")
    else()
      message(WARNING "Could not set core.hooksPath; git hooks not activated")
    endif()
  endif()
endif()

option(COVERAGE "Instrument binaries for llvm source-based coverage" OFF)

if(COVERAGE)
  add_compile_options(-fprofile-instr-generate -fcoverage-mapping -O0 -g)
  add_link_options(-fprofile-instr-generate)
endif()

add_subdirectory(common)
add_subdirectory(server)
add_subdirectory(gui)
add_subdirectory(ia)

option(BUILD_TESTS "Build unit tests" OFF)

if(BUILD_TESTS)
  find_package(GTest QUIET)
  if(NOT GTest_FOUND)
    include(FetchContent)
    FetchContent_Declare(
      googletest
      URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.tar.gz
      URL_HASH SHA256=7b42b4d6ed48810c5362c265a17faebe90dc2373c885e5216439d37927f02926
      DOWNLOAD_EXTRACT_TIMESTAMP ON
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
  endif()
  enable_testing()
  add_subdirectory(tests)
endif()

find_program(CLANG_FORMAT NAMES clang-format-20 clang-format)
if(CLANG_FORMAT)
  add_custom_target(format
    COMMAND ${CLANG_FORMAT} -i ${ALL_SOURCE_FILES}
    COMMENT "Formatting code with ${CLANG_FORMAT}..."
  )
  add_custom_target(format-check
    COMMAND ${CLANG_FORMAT} --dry-run -Werror ${ALL_SOURCE_FILES}
    COMMENT "Checking code format with ${CLANG_FORMAT}..."
  )
endif()

find_program(CLANG_TIDY NAMES clang-tidy-20 clang-tidy)
if(CLANG_TIDY)
  add_custom_target(tidy
    COMMAND ${CLANG_TIDY} ${ALL_SOURCE_FILES} -p ${CMAKE_BINARY_DIR}
      --extra-arg=-Wno-unknown-warning-option
    COMMENT "Linting code with ${CLANG_TIDY}..."
  )
endif()
