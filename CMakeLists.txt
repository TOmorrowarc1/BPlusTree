cmake_minimum_required(VERSION 3.10)
project(TicketSystem)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 包含目录
include_directories(
    include/storage
    include/index
)

# 查找所有源文件（根据需要调整）
set(TEST_SOURCES 
    src/test/bpm_test.cpp
    src/storage/buffer_pool_manager.cpp
    src/storage/disk_manager.cpp
    src/storage/frame_manager.cpp
)

add_executable(code ${TEST_SOURCES})

target_compile_options(code PRIVATE -O3 -Wall)