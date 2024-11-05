cmake_minimum_required(VERSION 3.28)

project(SFML_ass2)

# -fsanitize=address 地址消毒
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -g -O0 -fsanitize=address -ftest-coverage -fprofile-arcs")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -g -O0")

# 查找 SFML 库（确保 SFML 安装路径在系统环境中）
find_package(SFML 2.6 COMPONENTS system window graphics REQUIRED)

#  exe
add_executable(sfml_demo
    ${PROJECT_SOURCE_DIR}/main.cpp
)

# exe 依赖
target_link_libraries(sfml_demo
    PRIVATE
    sfml-system
    sfml-window
    sfml-graphics
)

target_compile_features(sfml_demo
    PRIVATE
    cxx_std_17
)

# exe include
target_include_directories(sfml_demo
    PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)

# 可执行文件输出路径
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/output/bin)
# 库输出路径
set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/output/lib)
