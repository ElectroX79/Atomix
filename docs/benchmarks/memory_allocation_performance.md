# Memory Allocation Performance

## Question

How does the current memory allocation strategy behave
as the DataTable grows?

## Compared strategies

### Legacy

Description: 
- aligned allocation for small allocations
- mmap-backed allocation for larger allocations
- 64 KiB chunking
- adaptive allocation strategy

To check that legacy code version, go to the Git branch `legacy_memory_allocation`

### Simplified
Description:
- aligned allocation
- 64 KiB chunking for larger allocations

To check that code version, go to the Git branch `checkpoint_memory_allocation`

## Measurements

We measure:

- allocation + deallocation
- sequential access

using the same DataTable workload (see test/benchmark/benchmark_mem_route.cpp)

## Results 

### Hardware and conditions 

- CPU: AMD Ryzen 7 7435HS
- RAM: 24 GB DDR5
- OS: Fedora Linux
- Kernel: 7.1.5
- Compiler: GCC 16.1.1 (C++26)
- Optimization: -O2


### Result of the measures



The columns are created from the following templates:

- Int32 = {1, 2, 3}
- Float64 = {1.5, 2.5, 3.5}
- Bool = {1, 0, 1} (stored as uint8_t, not as a bit mask)

The table is composed of three columns:
1. Int32 
2. Float64
3. Bool

Each column is constructed by sequentially repeating its corresponding template.

The repetition is set by the variable `mult`:



| Workload             | Operation         |    Legacy |   Simplified |     Change |
| -------------------- | ----------------- | --------: | --------: | ---------: |
| Small (`mult = 256`) | Alloc + dealloc   |   3.09 µs |   1.62 µs | **−47.6%** |
|                      | Sequential access |   4.80 µs |   2.45 µs | **−48.9%** |
| Medium (`mult = 8K`) | Alloc + dealloc   | 334.67 µs |  37.18 µs | **−88.9%** |
|                      | Sequential access |  80.80 µs | 109.79 µs | **+35.2%** |
| Big (`mult = 128K`)  | Alloc + dealloc   |   3.30 ms | 894.52 µs | **−72.9%** |
|                      | Sequential access |  1.933 ms |  1.938 ms |  **+0.3%** |
| Huge (`mult = 8M`)   | Alloc + dealloc   | 513.13 ms | 179.73 ms | **−65.0%** |
|                      | Sequential access | 123.99 ms | 123.37 ms |  **−0.5%** |


Note: 1K = 1024 and 1M = 1024k <br>
Note 2: To calculate the storage needed: storage = mult * (3 * 4 + 3 * 8 + 3 * 1) = mult * (39) [bytes]

---

Detailed profiling (with perf linux):

| Workload | Operation       | Strategy | Cache misses |   Cycles | Minor faults | Major faults |
| -------- | --------------- | -------- | -----------: | -------: | -----------: | -----------: |
| Small    | Alloc + dealloc | Legacy   |    1,072,711 |  3.854 B |      105,280 |            0 |
|          |                 | Simplified  |    1,009,139 |  4.482 B |      110,279 |            0 |
| Small    | Access          | Legacy   |    1,059,193 |  4.148 B |      105,283 |            0 |
|          |                 | Simplified  |      948,201 |  4.291 B |      110,280 |            0 |
| Medium   | Alloc + dealloc | Legacy   |    2,680,399 |  3.781 B |      197,874 |            0 |
|          |                 | Simplified  |   20,856,706 |  8.221 B |      210,348 |            0 |
| Medium   | Access          | Legacy   |    2,206,742 |  8.094 B |      210,372 |            0 |
|          |                 | Simplified  |    2,223,128 |  7.895 B |      220,436 |            0 |
| Big      | Alloc + dealloc | Legacy   |    4,020,100 |  7.525 B |      425,590 |            0 |
|          |                 | Simplified  |   15,018,761 |  8.323 B |      223,481 |            0 |
| Big      | Access          | Legacy   |    2,171,706 |  8.703 B |      221,152 |            0 |
|          |                 | Simplified  |    2,110,947 |  8.680 B |      221,252 |            0 |
| Huge     | Alloc + dealloc | Legacy   |  168,269,068 | 56.107 B |   20,912,218 |            0 |
|          |                 | Simplified  |  370,740,891 | 59.306 B |    3,760,070 |            0 |
| Huge     | Access          | Legacy   |   12,692,817 | 61.529 B |      429,674 |            0 |
|          |                 | Simplified  |   13,700,337 | 61.309 B |      438,771 |            0 |


---
Process resources (/usr/bin/time -v)

| Workload | Operation       | Strategy | User time | System time | Elapsed |        Max RSS | Involuntary CS |
| -------- | --------------- | -------- | --------: | ----------: | ------: | -------------: | -------------: |
| Small    | Alloc + dealloc | Legacy   |    1.83 s |      0.42 s |  2.28 s |      316.4 MiB |            238 |
|          |                 | Simplified  |    1.77 s |      0.40 s |  2.18 s |      316.9 MiB |            106 |
| Small    | Access          | Legacy   |    1.91 s |      0.40 s |  2.35 s |     316.98 MiB |            385 |
|          |                 | Simplified  |    1.88 s |      0.38 s |  2.29 s |     316.71 MiB |            295 |
| Medium   | Alloc + dealloc | Legacy   |    1.72 s |      0.65 s |  2.39 s |     316.70 MiB |            220 |
|          |                 | Simplified  |    1.86 s |      0.36 s |  2.24 s |     316.64 MiB |             87 |
| Medium   | Access          | Legacy   |    1.91 s |      0.40 s |  2.33 s |     316.95 MiB |            126 |
|          |                 | Simplified  |    1.79 s |      0.36 s |  2.19 s |     317.25 MiB |            248 |
| Big      | Alloc + dealloc | Legacy   |    1.75 s |      0.82 s |  2.60 s |     316.67 MiB |             78 |
|          |                 | Simplified  |    1.85 s |      0.39 s |  2.32 s |     316.59 MiB |            210 |
| Big      | Access          | Legacy   |    1.98 s |      0.39 s |  2.39 s |     326.74 MiB |            128 |
|          |                 | Simplified  |    2.02 s |      0.37 s |  2.45 s |     326.71 MiB |            496 |
| Huge     | Alloc + dealloc | Legacy   |   12.03 s | **39.26 s** | 51.76 s | **644.39 MiB** |      **2,702** |
|          |                 | Simplified  |   13.08 s |  **6.98 s** | 20.34 s | **629.99 MiB** |        **787** |
| Huge     | Access          | Legacy   |   13.99 s |      0.79 s | 14.88 s |     667.88 MiB |            560 |
|          |                 | Simplified  |   13.93 s |      0.74 s | 14.84 s |     673.33 MiB |            426 |

---


**Note**: You can reproduce the measurements by running the
benchmark script `scripts/benchmarks/mem_route_benchmark.sh` or directly executing
the `mem_route_benchmarks` CMake executable (this second does not include profiling and 
resource usage measurements)

## Comment on the results

The simplified strategy greatly reduces the allocation and deallocation time,
especially for larger workloads. For Huge, the time decreases from 513.1 ms to 
179.7 ms (−65.0%), while sequential access remains almost unchanged (−0.5%).

The profiling results show a reduction in operating-system-related overhead.
For huge allocation and deallocation, system time goes from 39.26 s to 6.98 s, 
while minor page faults decrease from 20.9 million to 3.8 million. Maximum RSS remains
similar, maybe the improvement is not simply caused by using less memory.

These results suggest that the legacy strategy introduces additional overhead, 
which could be potentially related to its size-dependent 
use of a custom mmap handler and chunking. 

