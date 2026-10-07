# Internals
> **Partially deprecated since ADR-0006 and ADR-0007**
>
> **Warning:** This page is partially deprecated and may be changed in the future.
>
> See [ADR-0006](../../adr/adr-0006-batching.md)
> and [ADR-0007](../../adr/adr-0007-persistence-ram-disk.md) for more details
>
This section describes the internal components and implementation details of
Atomix.

These details are implementation-specific and may change as the architecture
evolves.

## Sections
### Data
- [Column](data/column.md)
- [DataType](data/data_type.md)
- [DataTable](data/data_table.md)
### Memory
- [Buffer](mem/buffer.md)
- [Aligned allocator](mem/aligned_allocator.md)
- [Virtual Memory](mem/vmem.md)
- [Memory route](mem/mem_route.md)