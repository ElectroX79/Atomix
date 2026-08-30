# Buffer
Buffer is a class that owns a block of memory, that means
that it is responsible for acquiring and freeing automatically the memory (RAII).

## Composition
Buffer is composed of:

- `uint8_t* begin`: a pointer to the beginning of the memory block.
- `size_t size`: the size in bytes of the memory block.
- `AllocationType alloc_t_`: set how the memory block was allocated, will be used to free it.

## Behavior

When a buffer is constructed, it will call `Mem_route` to allocate the memory block.
(See [Mem_route](mem_route.md)). When the buffer reaches the end of its lifetime, the destroyer
will call `Mem_route` to free the memory block, before destructing the buffer.