#!/usr/bin/env bash
set -e

cmake -B build -G Ninja
cmake --build build --target mem_route_benchmark
#GitHub workflow do not allow to change the perf_event_paranoid
#perf_flags="cache-misses,cycles,minor-faults,major-faults"
executable_route="./build/mem_route_benchmark"
# file_name1="benchmark_result_perf.txt"
# file_name2="benchmark_result_resources.txt"

#run_perf(){
#    local tag="$1"
#
#   perf stat -e "$perf_flags" \
#        "$executable_route" "$tag"
#}

run_resources(){
    local tag="$1"

    /usr/bin/time -v \
        "$executable_route" "$tag"
}



tags=(
    "[mult_256][alloc + dealloc]"
    "[mult_256][access]"
    "[mult_8k][alloc + dealloc]"
    "[mult_8k][access]"
    "[mult_128k][alloc + dealloc]"
    "[mult_128k][access]"
    "[mult_8m][alloc + dealloc]"
    "[mult_8m][access]"
)

#: > "$file_name1"
#: > "$file_name2"

#GitHub workflow do not allow to change the perf_event_paranoid
#for tag in "${tags[@]}"; do
#    run_perf "$tag"
#done


for tag in "${tags[@]}"; do
    run_resources "$tag"
done







