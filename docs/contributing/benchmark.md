# Benchmarks

In Atomix you can create two types of benchmarks: **normal benchmarks** and **deep benchmarks**.

A deep benchmark is a **superset of a normal benchmark**. It performs the same benchmark, but also collects additional information such as CPU cycles, cache misses or page faults.

## Normal benchmarks

Normal benchmarks are used for quick performance measurements.

They should use the Catch2 benchmark framework.

Use a normal benchmark when execution time is enough to evaluate the change.

## Deep benchmarks

Deep benchmarks are normal benchmarks with additional performance measurements.

They can use any suitable performance tool. For example, Linux users can use `perf` to collect hardware and operating system performance counters such as:

* CPU cycles;
* cache misses;
* page faults;
* and other available counters.

When creating a deep benchmark, a Bash script must be provided to run the benchmark and store its output in a file.

For example:

```bash
#!/usr/bin/env bash
set -e

cmake -B build
cmake --build build --target benchmark_mem_route

perf stat -e cache-misses,cycles,minor-faults,major-faults \
    ./build/benchmark_mem_route \
    "[mult_1m][access]" \
    > benchmark_result.txt 2>&1
```

The script should make the deep benchmark easy to reproduce.




## Which one should I use?

Use a **normal benchmark** when execution time is enough.

Use a **deep benchmark** when you need additional information to understand the performance of the implementation.

A deep benchmark should still provide the normal benchmark result. The additional measurements are used to understand **why** the performance changes.

## Documenting benchmarks

Benchmarks should document enough information to understand and reproduce the results.

A benchmark report should include:

- The question is being investigated;
- The implementations or strategies being compared;
- The workload being used;
- The hardware and software configuration;
- The measured results;
- Relevant observations or conclusions.

Check an example: [Memory Allocation Performance](../benchmarks/memory_allocation_performance.md)