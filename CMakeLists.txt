
cmake_minimum_required(VERSION 3.15)
project(Atomix VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# add_link_options(-fsanitize=address,undefined)

add_compile_options(-march=native -Wall -Wextra -fno-omit-frame-pointer -g -O2)


#Library AtomixCore
add_library(AtomixCore)
target_sources(AtomixCore PRIVATE
        src/mem_route.cpp
        src/data_table.cpp
)
target_include_directories(AtomixCore PUBLIC ${PROJECT_SOURCE_DIR}/include)



#executable test_data_table
add_executable(data_table_test
external/catch2/catch_amalgamated.cpp
test/component_test/data_table_test.cpp
)
target_include_directories(data_table_test PRIVATE include external/catch2)
target_link_libraries(data_table_test PRIVATE AtomixCore)


#executable test_mem_route_buffer
add_executable(mem_route_test
        external/catch2/catch_amalgamated.cpp
        test/component_test/mem_route_test.cpp
)

target_include_directories(mem_route_test PRIVATE include external/catch2)
target_link_libraries(mem_route_test PRIVATE AtomixCore)


#executable benchmark_mem_route
add_executable(mem_route_benchmark
        external/catch2/catch_amalgamated.cpp
        test/benchmark/mem_route/benchmark_mem_route.cpp
)

target_include_directories(mem_route_benchmark PRIVATE include external/catch2)
target_link_libraries(mem_route_benchmark PRIVATE AtomixCore)