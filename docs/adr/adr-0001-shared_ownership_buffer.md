# Shared ownership for class buffer (ADR-0001)

## Status
Accepted


## Context
Class [Column](../internals/data/column.md) previously stored [Buffer](../internals/mem/buffer.md) directly.


## Decision
A column now stores a shared pointer to `Buffer`.

## Motivation

- Enable shallow copy instead of deep copy.
- Simplify memory ownership.

## Consequences

### Advantages

- Potential CoW
- Shallow copy
- Less manual memory ownership, more oriented to RAII.

### Disadvantages

- Additional metadata.
- Atomic operations.


