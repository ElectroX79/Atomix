
cmake_minimum_required(VERSION 3.15)
project(Atomix VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(-march=native -Wall -Wextra -g -O2)

function(enable_sanitizers target)
    target_compile_options(${target} PRIVATE
            -fsanitize=address,undefined
            -fno-omit-frame-pointer
    )

    target_link_options(${target} PRIVATE
            -fsanitize=address,undefined
    )
endfunction()


function(create_atomix_core target)
    add_library(${target})
    target_sources(${target} PRIVATE
            src/mem_route.cpp
            src/data_table.cpp
    )
    target_include_directories(${target} PUBLIC ${PROJECT_SOURCE_DIR}/include)
endfunction()

function(create_test_instance target target_source libraries)
    add_executable(${target}
            external/catch2/catch_amalgamated.cpp
            ${target_source}
    )
    target_include_directories(${target} PRIVATE include external/catch2)
    target_link_libraries(${target} PRIVATE ${libraries})

endfunction()


#Library AtomixCore
create_atomix_core(AtomixCore)

#Library AtomixCoreSanitized
create_atomix_core(AtomixCoreSanitized)
enable_sanitizers(AtomixCoreSanitized)


#executable data_table_test_
create_test_instance(data_table_test test/component_test/data_table_test.cpp AtomixCoreSanitized)
enable_sanitizers(data_table_test)


#executable mem_route_test
create_test_instance(mem_route_test test/component_test/mem_route_test.cpp AtomixCoreSanitized)
enable_sanitizers(mem_route_test)


#executable mem_route_benchmark
create_test_instance(mem_route_benchmark test/benchmark/mem_route/mem_route_benchmark.cpp AtomixCore)

