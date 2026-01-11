cmake_minimum_required(VERSION 3.16)

project(mp_oi_library LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/releases/download/v1.16.0/googletest-1.16.0.tar.gz
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(googletest)

enable_testing()

file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS test/main.cpp)

add_executable(mp_oi_library_tests ${TEST_SOURCES})

target_include_directories(mp_oi_library_tests PRIVATE
  ${CMAKE_SOURCE_DIR}
)

target_compile_options(mp_oi_library_tests PRIVATE
  -Wall
  -Wextra
  -Wshadow
  -Wpedantic
  -Wno-unused-parameter
  -O2
)

target_link_libraries(mp_oi_library_tests PRIVATE
  GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(mp_oi_library_tests)
