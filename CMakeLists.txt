cmake_minimum_required(VERSION 3.0)

# std=c++17
set(CMAKE_CXX_STANDARD 17)

project(shadow_stl)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

Include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.0.1 # or a later release
)

FetchContent_MakeAvailable(Catch2)

# add include path
include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/src/include)
include_directories(${CMAKE_SOURCE_DIR}/src/allocator)
include_directories(${CMAKE_SOURCE_DIR}/src/iterator)
include_directories(${CMAKE_SOURCE_DIR}/src/algorithm)
include_directories(${CMAKE_SOURCE_DIR}/src/container)


# add source
add_executable(tests ${CMAKE_SOURCE_DIR}/test/stl_threads_test.cc
                      ${CMAKE_SOURCE_DIR}/test/stl_alloc_test.cc
                      ${CMAKE_SOURCE_DIR}/test/stl_construct_test.cc
                      ${CMAKE_SOURCE_DIR}/test/stl_iterator_test.cc)
add_executable(fake_test ${CMAKE_SOURCE_DIR}/src/test.cc)

target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)
