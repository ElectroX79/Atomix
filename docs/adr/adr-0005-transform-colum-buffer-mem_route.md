# Transform and linearize colum-buffer-mem_route (ADR-0005)

## Context 

In the past `Mem_route` was responsible for providing a chunked allocation in a
vector of buffers. `Buffer` only can be created by `mem_route` by default.

```text

Column
  │
  │ requests memory
  ▼
mem_route
  │
  │ creates
  ▼
vector<Buffer>
```

## Decision

- `Column` has the resposibility of creating the vector of `Buffer`, creating itself the buffers.
- `Buffer` calls mem_route to allocate when it was created and manages his own lifetime through RAII.
- `mem_route` routes to the right allocator depending on the allocation strategy.

```text

Column
│
│ creates
▼
vector<Buffer>
│
│ owns
▼
Buffer
│
│ requests allocation
▼
mem_route
│
│ routes
▼
Proper allocator
```

## Consequences
- No more awkwards circular dependencies between `Column` and 'mem_route'.


### Advantages 
- Clearer separation of functions and responsibilities.
- Better API for the developer.
- Direct and clearer DAG construction, avoiding linker errors.
- Easier to understand, debug and maintain.

### Disadvantages
- Less direct control.
- `mem_route` loses potential to preoptimize allocations, 
because the lack of control.

> **Note**: About the last point, there is no evidence for now that the system
can be preoptimized if mem_route is aware of the allocation strategy. It is just
**potential**.



