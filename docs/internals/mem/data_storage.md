# Data Storage and Chunks

`DataStorage` manages the logical allocation of memory regions within a file.

- It uses `FileManager` for physical storage management and returns `Chunk` descriptors for allocated regions.

---

## 1. Design and Lifecycle

Owns a `FileManager` object and stores the allocated data size: 
- **Logical Allocation:** Allocates contiguous regions and returns a `Chunk` containing the offset, size, and storage reference.
- **Alignment:** Rounds allocation sizes up to `default_alignment`.
- **Dynamic Capacity:** Uses `reserve()` to grow storage when an allocation exceeds the available capacity.
- **Cloning:** `clone()` duplicates the backing file and preserves the storage metadata.

---

## 2. Allocation & Addressing

`DataStorage` separates allocated data size from physical capacity. 
`allocate_chunk()` advances the allocation cursor and returns a descriptor for the reserved region.

Chunks store logical offsets rather than absolute pointers, allowing addresses to be resolved against the current mapping base.

---

## 3. Most Important Operations

- `begin()`: Returns the base address of the mapped storage.
- `capacity()`: Returns the backing file size.
- `data_size()`: Returns the current allocation size.
- `reserve()`: Increases capacity without changing the allocation cursor.
- `resize()`: Resizes the backing file and clamps `data_size_` if necessary.
- `safe_resize()`: Resizes storage but aborts if the new size would truncate allocated data.
- `allocate_chunk()`: Allocates an aligned region and returns its descriptor.
- `clone()`: Creates a copy of the storage and its allocation metadata.

---


## 4. About chunk
Chunk is a descriptor for a contiguous region of memory. 

> To learn more about physical file management, see [FileManager](file_manager.md).