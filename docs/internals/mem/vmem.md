# Virtual memory
Vmem is a manager for virtual memory, currently just for POSIX systems through the `mmap` syscall.
More information about mmap can be found [here](https://man7.org/linux/man-pages/man2/mmap.2.html).

## Current state
For now, Vmem is not being used (but was used and considered) because it introduces a
lot of complexity. You can learn more about it [here](../../adr/adr-0003-removal_vmem_chunk.md).

Despite the current state, VMem is being studied for another use, see [Custom allocator](../../future_features/proposals/custom_allocator.md).