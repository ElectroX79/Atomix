
# Custom Allocator

The current benchmarks show significant overhead in memory allocation and
deallocation for large workloads. A possible solution is a custom allocator
specialized for Atomix's allocation patterns.

```text
        Custom Allocator
               │
        ┌──────┴──────┐
        ▼             ▼
     64 KiB         64 KiB
      chunk          chunk
        │             │
      used           free

```
The allocator would manage a larger memory region divided into 64 KiB
chunks, reducing the need for independent allocations.

Small allocations could continue using the existing aligned allocator.

## Goal
Reduce allocation and deallocation overhead while keeping data access
performance unchanged. This needs to be validated with specific benchmarks before 
definitive implementation.


## See also
[Benchmark](../../benchmarks/memory_allocation_performance.md): memory allocation performance deep benchmark




