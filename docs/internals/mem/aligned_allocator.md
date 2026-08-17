# Aligned Allocator

It is a simple wrapper around `aligned_alloc`, which is a posix-only function.
(See [aligned_alloc](https://man7.org/linux/man-pages/man3/aligned_alloc.3.html))

## Why a user-space allocator?
- It is portable.
- It is simple.
- It is suitable for small allocations and usually well-balanced for performance-usability trade-off.


## Why aligned?
- Makes SIMD easier to support.
- Provides predictable memory alignment.
- Better byte-level management.
