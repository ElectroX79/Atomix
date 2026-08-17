# Mem Route
> You need to understand [Buffer](buffer.md), [Aligned allocator](aligned_allocator.md)
and [Vmem](vmem.md) before continue reading.

The role of this component is to provide a unified interface to the low-level memory management 
and optimize the memory allocation strategy depending on the size of the allocation.

## Allocation strategy

`Mem_route` currently uses two allocation strategies:

- **Small allocations:** use [Aligned allocator](aligned_allocator.md).
- **Larger allocations:** use aligned allocation with 64 KiB chunking.

The caller receives a vector of shared pointers of `Buffer` (just one if the allocation is small), and
the size of each chunk (0 if it is not chunked).

## Why a routing layer?

The routing layer keeps allocation decisions separate from the components that
use memory.

For example:

```text
             DataTable
                 |
                 v
              Buffer
                 |
                 v
             Mem_route
             /       \
            v         v
   Aligned allocator  Chunked allocation

```