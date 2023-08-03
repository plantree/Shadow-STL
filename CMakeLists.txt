cmake_minimum_required(VERSION 3.0)

# std=c++17
set(CMAKE_CXX_STANDARD 17)

project(shadow_stl)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# add include path
include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/src/include)


# # 添加源文件
add_executable(callback src/test.cc)

# # 设置头文件查找路径
# target_include_directories(callback PRIVATE
#     ${CMAKE_SOURCE_DIR}
#     ${CMAKE_SOURCE_DIR}/functional
# )