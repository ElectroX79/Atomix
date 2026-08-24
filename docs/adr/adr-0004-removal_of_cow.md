# Removal of Copy-on-Write (ADR-0004)

## Status

Accepted

## Context

Previous versions used shared ownership of `Buffer` storage, allowing multiple `Buffer` instances to reference the same memory.

This allowed cheap shallow copies, but made ownership and in-place mutation less explicit and pretty much expensive.

## Decision

Remove the Copy-on-Write/shared-ownership mechanism.

`Buffer` now owns its storage directly. Copying performs a deep copy, while moving transfers ownership.

## Consequences

- Shallow copying is no longer available.
- Buffer ownership is explicit.
- Buffer mutation is cheaper.

### Advantages

- Simpler ownership model.
- In-place mutation without having to copy, allowing more natural and cheap cost.
- Move operations remain inexpensive.
- No significant performance impact in the evaluated operations (see benchmark [Buffer benchmarks](../benchmarks/cow_buffer_ownership.md)).

### Disadvantages

- Loss of cheap shallow copies.
- Copying requires allocating and copying the underlying storage.

## Observations

- Recommendation to see benchmark [Buffer benchmarks](../benchmarks/cow_buffer_ownership.md)
 The loss of shallow copying is an intentional trade-off for simpler ownership and in-place mutability.
