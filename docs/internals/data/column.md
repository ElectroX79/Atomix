# Column

A `Column` is the internal representation of a column in a `DataTable`.

Its responsibility is to store values of a single data type together with the metadata required to manage them.
## Buffer ownership

A `Column` stores its buffers in a `std::vector<std::shared_ptr<Buffer>>` where each buffer is a chunk.

Using a `std::vector` provides efficient sequential iteration while allowing the number of buffers to grow dynamically as the column expands.

Buffers are managed through `std::shared_ptr` because they may be shared by multiple `Column` instances. This enables future features such as Copy-on-Write without 
requiring the physical duplication of memory when a column is copied.


## Metadata

Besides the values, a `Column` contains the metadata which describes itself:

- **Name**: the column identifier.
- **Data type**: the type shared by every stored value.
- **Element count**: the number of logical elements contained in the column.
- **Size of each chunk**: the size of each buffer is homogenous except the last buffer, which could be smaller but always a valid modulo of the chunk size.
- **`vector<ListMetadata>`**: a dedicated array of metadata for the columns which are 
`DataType::List`. Each element corresponds directly to its respective buffer. (If vector.size() == 0,
the column has a fixed size datatype)


## What is  `ListMetadata ` ?
`ListMetadata ` is the metadata that describes how the variable data is organized in a 
specific chunk it is composed of:
- `std::vector<uint32_t> offsets`: the position in the chunk of each elemnent`List`
- `uint32_t last_used_byte`: number of bytes containing valid data in the chunk. 
It can be used to determine the unused padding at the end of the chunk.
- `DataType type`: sets the type of primitive data is being stored in a `List`


## Chunked columns

Large columns may be split into multiple buffers.

The `chunk_size` field describes this layout. A value of `0` indicates that the column is stored in a single allocation. Otherwise, it represents the size of each chunk.

## Design

`Column` is intentionally implemented as a nested class of `DataTable` (`atomix::DataTable::Column`).

It is considered an implementation detail rather than a public abstraction because the complexity of `Column`. Users interact exclusively with `DataTable`, while `Column` manages the storage and metadata of individual columns.


