cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_TESTS "Build tests" OFF)

project(banking)

file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/banking/*.cpp)

add_library(banking STATIC ${SOURCES})

target_include_directories(banking PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/banking>
  $<INSTALL_INTERFACE:include>
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fprofile-arcs -ftest-coverage")

if(BUILD_TESTS)
  enable_testing()
  add_subdirectory(third-party/gtest)

  file(GLOB TEST_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp")
  add_executable(check ${TEST_SOURCES})

  target_include_directories(check PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/banking
    ${CMAKE_CURRENT_SOURCE_DIR}/third-party/gtest/googletest/include
    ${CMAKE_CURRENT_SOURCE_DIR}/third-party/gtest/googlemock/include
  )

  target_link_libraries(check banking gtest gtest_main gmock_main)

  add_test(NAME check COMMAND check)
endif()

install(TARGETS banking
  EXPORT banking-config
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/banking/ DESTINATION include
  FILES_MATCHING PATTERN "*.h"
)

install(EXPORT banking-config DESTINATION cmake)
