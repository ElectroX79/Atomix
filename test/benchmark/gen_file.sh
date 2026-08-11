cmake -B build
cmake --build build --target benchmark_mem_route

perf_flags="cache-misses,cycles,minor-faults,major-faults"
executable_route="./build/benchmark_mem_route"
file_name="benchmark_result.txt"

touch ./"$file_name"
perf stat -e "$perf_flags" "$executable_route" "[mult_256][alloc + dealloc]" >> "$file_name" 2>&1
perf stat -e "$perf_flags" "$executable_route" "[mult_256][access]" >> "$file_name" 2>&1
perf stat -e "$perf_flags" "$executable_route" "[mult_8k][alloc + dealloc]" >> "$file_name" 2>&1
perf stat -e "$perf_flags" "$executable_route" "[mult_8k][access]" >> "$file_name" 2>&1
perf stat -e "$perf_flags" "$executable_route" "[mult_128k][alloc + dealloc]" >> "$file_name" 2>&1
perf stat -e "$perf_flags" "$executable_route" "[mult_128k][access]" >> "$file_name" 2>&1
perf stat -e "$perf_flags" "$executable_route" "[mult_8m][alloc + dealloc]" >> "$file_name" 2>&1
perf stat -e "$perf_flags" "$executable_route" "[mult_8m][access]" >> "$file_name" 2>&1



