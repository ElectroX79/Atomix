cmake -B build
cmake --build build --target benchmark_mem_route

perf_flags="cache-misses,cycles,minor-faults,major-faults"
executable_route="./build/benchmark_mem_route"
file_name="benchmark_result.txt"

touch ./"$file_name"
perf stat -e "$perf_flags" "$executable_route" "[mult_k][alloc + dealloc]" >> "$file_name" 2>&1
perf stat -e "$perf_flags" "$executable_route" "[mult_k][access]" >> "$file_name" 2>&1
perf stat -e "$perf_flags" "$executable_route" "[mult_32k][alloc + dealloc]" >> "$file_name" 2>&1
perf stat -e "$perf_flags" "$executable_route" "[mult_32k][access]" >> "$file_name" 2>&1
perf stat -e "$perf_flags" "$executable_route" "[mult_128k][alloc + dealloc]" >> "$file_name" 2>&1
perf stat -e "$perf_flags" "$executable_route" "[mult_128k][access]" >> "$file_name" 2>&1
perf stat -e "$perf_flags" "$executable_route" "[mult_1m][alloc + dealloc]" >> "$file_name" 2>&1
perf stat -e "$perf_flags" "$executable_route" "[mult_1m][access]" >> "$file_name" 2>&1




