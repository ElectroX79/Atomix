# Memory management

Memory management is one of the core components of Atomix. It is not just an allocator, it is
a module responsible for enabling an efficient and consistent data sharing, a predictable lifetime of the memory
and a storage model for the entire engine.
---
## Why is memory management important?
Not all memory allocations could benefit from the same memory managing strategy. Each one needs a more optimized approach 
to reach their optimal performance.

For that reason Atomix needs a dynamic memory manager that could fit the different needs, because
different use patterns need different strategies.
---
## Allocation criteria

Memory allocations are classified according to a set of criteria that describe their expected usage. These criteria determine which allocation strategy should be used.

The most important factors are:

- Allocation size.
- Expected lifetime.
- Allocation frequency.
- Sharing requirements.
---
## Current memory allocation criteria

Due to the early stage of development, Atomix currently considers only one allocation criterion;
the allocation size, which will determine if the allocation needs to be chunked.

The current implementation is intentionally simple and serves as the foundation for future memory management improvements. More specialized strategies will be introduced as the engine evolves.

---
To learn more about the memory management implementation, you can consult [Memory route](../internals/mem/mem_route.md)














