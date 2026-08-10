
cmake_minimum_required(VERSION 3.15)
project(Atomix VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_link_options(-fsanitize=address,undefined)
add_compile_options(-march=native -Wall -Wextra -fsanitize=address,undefined -fno-omit-frame-pointer -g -O2)


#Library AtomixCore
add_library(AtomixCore)
target_sources(AtomixCore PRIVATE
        src/mem_route.cpp
        src/data_table.cpp
)
target_include_directories(AtomixCore PUBLIC include)



#executable unit_test_data_table
add_executable(unit_test_data_table
external/catch2/catch_amalgamated.cpp
test/data_table_unit_test.cpp
)
target_include_directories(unit_test_data_table PRIVATE include external/catch2)
target_link_libraries(unit_test_data_table PRIVATE AtomixCore)


#executable test_mem_route_buffer
add_executable(test_mem_route_buffer
        external/catch2/catch_amalgamated.cpp
        test/mem_route_test.cpp
)

target_include_directories(test_mem_route_buffer PRIVATE include external/catch2)
target_link_libraries(test_mem_route_buffer PRIVATE AtomixCore)


#executable benchmark_mem_route
add_executable(benchmark_mem_route
        external/catch2/catch_amalgamated.cpp
        test/benchmark/benchmark_mem_route.cpp
)

target_include_directories(benchmark_mem_route PRIVATE include external/catch2)
target_link_libraries(benchmark_mem_route PRIVATE AtomixCore)