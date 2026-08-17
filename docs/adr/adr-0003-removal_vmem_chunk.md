# Removal of vmem_chunk (ADR-0003)

## Status
Accepted

## Context

Previous versions implemented chunking by reserving a large virtual memory region using `mmap`.

The memory was logically divided into chunks with the help of `vmem_handler` and the `Buffer` themself, allowing multiple `Buffer` to reference different sections of the same virtual allocation.


## Decision
Remove the virtual-memory-based chunking mechanism.
Now each chunk is allocated directly with `aligned_allocator`.


## Consequences
- Chunking systems is now physical, not logical.

### Advantages
- Simpler implementation.
- Reduced maintenance cost.
- No need to create a logical chunking system.
- Lower overhead under the evaluated workloads (see benchmark [Vmem vs aligned_allocator](../benchmarks/vmem_vs_align_alloc.md))

### Disadvantages

- More individual system allocations.
- Loss of large contiguous virtual address reservations.

## Observations

- Recommendation to see benchmark [Vmem vs aligned_allocator](../benchmarks/vmem_vs_align_alloc.md)
- This change is a temporal change, future implementation of a [custom allocator](../future_features/potential_features_changes/custom_allocator.md) will be used
