# Benchmark of sum column aggregation

## Purpose

The purpose of this benchmark is to compare the performance of this library's aggregation operation against external reference implementations.

## Methodology

Atomix will execute the `col_sum()` operation, using **std::chrono** for timing measurements and **perf record** 
for performance profiling ([What's perf?](https://man7.org/linux/man-pages/man1/perf.1.html)).

In addition to Atomix, we will compare its performance against the following implementation under similar conditions:

- A simple custom implementation in C using `intrinsics` to perform a column sum.

The goal of this implementation is to achieve the maximum possible CPU performance under the same conditions, 
but without the abstraction overhead that a data engine could add.

## Conditions

### Hardware

- Ryzen 7 7435HS
- 24 GB DDR5 RAM
- Maximum frequency forced to approximately 4.5 GHz

### Software

- GCC 16.2.1
- Fedora Linux
- Relevant compiler flags: `-march=native -g -O3 -fno-omit-frame-pointer`

### Program conditions

- 1024 × 1024 × 500 elements per column.
- Data table schema: `int32, float64, char`.
- All data was preloaded into RAM (resident in RSS).
- Single-threaded execution.
- No cache warmup*.
- Almost all data was enabled (the bitmasks were dense).

### Variants

Two data types were analyzed:

- 32-bit signed integers (`int32`, 2 GiB).
- 64-bit floating-point numbers (`float64`, 4 GiB).

## Reproducibility

To use the same code, switch to the `frozen/benchmarks_agregation` (*Git*) branch.

Then, to run the same program of Atomix used in this benchmark:

1. Navigate to `./demos/scripts`.
2. Follow the instructions in the `README.md` file inside that directory.

And to run the C implementation, execute the following commands:

```bash
cd test/benchmark/aggregation/c_comparison
make
./float64_array_sum
./int32_array_sum
make clean
```

## Results

### Atomix

| Data Type | Elements | Size (GiB / GB) | Time (s) | Processing Rate | Bandwidth (GiB/s / GB/s) |
|:--|--:|--:|--:|--:|--:|
| `int32` | 524,288,000 | 1.95 GiB / 2.10 GB | 0.1664 s | 3,150.54 M elem/s | 11.74 GiB/s / 12.60 GB/s |
| `float64` | 524,288,000 | 3.91 GiB / 4.19 GB | 0.2361 s | 2,220.31 M elem/s | 16.54 GiB/s / 17.76 GB/s |

### C implementation (intrinsics)

| Data Type | Elements | Size (GiB / GB) | Bandwidth (GiB/s / GB/s) | Relative Performance (Atomix/C) |
|:--|--:|--:|--:|--:|
| `int32` | 524,288,000 | 1.95 GiB / 2.10 GB | ~29.42 GiB/s / ~31.59 GB/s | 39.90% efficiency |
| `float64` | 524,288,000 | 3.91 GiB / 4.19 GB | ~30.43 GiB/s / ~32.67 GB/s | 54.37% efficiency |

## Details observed with perf

- Technically, using multiple accumulators helps mitigate latency and take advantage of superscalar execution. This approach worked well for `float64`, since floating-point operations have higher latency and dedicated hardware. In fact, the `float64` variant was further optimized by increasing the number of accumulators from 4 to 8.

  However, this caused a problem with `int32`: `perf` reported that this variant was experiencing register spilling to the stack. For that reason, the number of accumulators was reduced to 1.

- After analyzing the `perf` data, we found that the following operation was more expensive than expected (9.207% for `float64` and 24.43% for `int32`; see the appendix):

  ```cpp
  const auto bit_mask = (batch_data.row_mask & batch_mask & col_to_op.validity_mask)
  ```
 
  As a result, these operations were re-optimized to improve their efficiency 
 (specifically with loop fusion and by explicitly indicating that the relevant pointers do not alias)

## Analysis

There is a significant performance gap between Atomix and the custom implementation.

This difference can mainly be described by the following factors:

1. Creating a new bitmask from multiple data sources (table, `proxy_select`, and `validity_mask`).
2. The C implementation uses direct assembly-level instructions to execute the operation.
3. Checking data validity and invariants is not always trivial.
4. Although the data has alignment and local contiguity, Atomix does not have absolute contiguity between chunks.
   (This could penalize the TLB and the prefetcher).
5. Atomix may be suboptimized.

## Observations

- We can assume that there is no significant cache warmup of the data (although metadata may be cached)
since the previous operation (insertion) finished at the tail of the dataset, also this operation starts reading 
from the head. In addition, the dataset is larger than the CPU cache.

___

## Appendix


### Figure 1 — Current version

```text
|--12.49%--atomix::data::TypeTraits@atomix.data.data_type<(atomix::data::DataType@atomix.data.data_type)1>::type atomix::data::SelectProxy@atomix.data.sel>
|          |
|          |--11.19%--atomix::data::TypeTraits@atomix.data.data_type<(atomix::data::DataType@atomix.data.data_type)1>::type atomix::data::SelectProxy@atom>
|          |
|           --1.15%--atomix::BitMask@atomix.bit_mask::operator&(atomix::BitMask@atomix.bit_mask const&) const
|
|--8.80%--atomix::data::TypeTraits@atomix.data.data_type<(atomix::data::DataType@atomix.data.data_type)0>::type atomix::data::SelectProxy@atomix.data.sele>
|          |
|          |--6.50%--atomix::data::TypeTraits@atomix.data.data_type<(atomix::data::DataType@atomix.data.data_type)0>::type atomix::data::SelectProxy@atomi>
|          |
|           --2.15%--atomix::BitMask@atomix.bit_mask::operator&(atomix::BitMask@atomix.bit_mask const&) const
```

### Note

The absolute cost of each `operator&` operation is:

- Approximately 1.15% for the `float64` variant.
- Approximately 2.15% for the `int32` variant.

> Note that the cost of each `operator&` does not include the cost of preparing or allocating the bitmask.

Based on these values, we can estimate the cost within the local `col_sum()` function as follows:

- `float64`: 1.15 / 12.49 = 0.09207, or **9.207%** of the total cost.
- `int32`: 2.15 / 8.80 = 0.2443, or **24.43%** of the total cost.

---