# Buffer Ownership and Copy Performance (COW comparison)

## Question

What is the performance impact of replacing shared ownership of `Buffer` (COW)
with explicit ownership and in-place mutability?

## Compared strategies

### Legacy
Available in branch `frozen/cow_reference`

Description:
- `Buffer` objects are managed through `std::shared_ptr`
- shallow copies are supported through shared ownership
- buffer data can be shared between multiple owners

### Simplified
Use as a reference the branch `refactor/removal_cow_and_shared_ptr`

Description:
- `Buffer` objects use explicit ownership
- `Buffer` is stored directly instead of through `std::shared_ptr`
- shallow copying is intentionally removed
- buffers can be modified in-place instead of using shared ownership for COW

## Measurements

We measure:

- creation + destruction
- shallow copying (legacy only)
- deep copying
- moving
- sequential access

## Results

### Hardware and conditions

- CPU: AMD Ryzen 7 7435HS
- RAM: 24 GB DDR5
- OS: Fedora Linux
- Compiler: GCC 16.1.1 (C++26)
- Optimization: -O2

### Result of the measurements

| Operation | Legacy | Simplified | Change |
| --------- | -----: | ---------: | -----: |
| Creation + destruction | ~55–61 µs | ~55–78 µs | same order of magnitude |
| Shallow copy | ~66–74 µs | N/A | intentionally removed |
| Deep copy | ~22 ms | ~22 ms | ~no significant change |
| Move | ~4.0–4.7 µs | ~4.1–4.2 µs | ~no significant change |
| Sequential access | ~64–70 ms | ~66–70 ms | no significant regression observed |

The measurements were repeated across multiple executions. Small
differences between runs are expected due to system and benchmark
variability.

**Note**: You can reproduce the measurements by running the
benchmark script `scripts/benchmarks/buffer_benchmarks.sh` or directly executing
the `buffer_benchmarks` CMake executable.

## Comment on the results

The simplified ownership model removes the cheap shallow-copy operation,
which was provided by `std::shared_ptr`. This is an intentional trade-off:
shallow copying is no longer part of the `Buffer` ownership model, allowing
buffers to have explicit ownership and to be modified in-place.

The remaining operations don't show a significant performance impact.
Deep copying and moving remain in the same performance interval as the legacy
implementation, while sequential access remains in the same order of
magnitude.

Creation and destruction also remain in the same general performance range,
although individual measurements show some variability between executions.

These results indicate that removing shared ownership does not introduce a
significant performance penalty for the operations retained by the new
`Buffer` design.

