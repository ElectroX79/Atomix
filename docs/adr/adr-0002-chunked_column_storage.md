# Chunked Column Storage (ADR-0002)

## Status
Accepted


## Context
Class [Column](../internals/data/column.md) previously uses a single contiguous buffer.


## Decision
A column is represented as a sequence of `Buffer` objects.


## Motivation

- Eliminate large reallocations.
- Support incremental growth.
- Prepare the  model for Copy-on-Write.

## Consequences

### Advantages

- Incremental growth.
- Simpler memory ownership.
- Better scalability.

### Disadvantages

- Additional metadata.
- Slightly more complex iteration
- More atomic operations.

## Observations
- Need to benchmark the trade-off and decide a sweet point between atomic operations and copying.