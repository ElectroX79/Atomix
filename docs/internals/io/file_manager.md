

# File Manager

`FileManager` is Atomix's low-level storage abstraction and manages file descriptors and memory mappings.

---

## 1. Design & Lifecycle

- **Move-Only Ownership:** Copy operations are deleted to prevent accidental duplication of resource ownership. Move operations transfer the managed resources.
- **Memory-Mapped Storage:** Uses `mmap(MAP_SHARED)` to access file contents directly.
- **Dynamic Resizing:** Uses `ftruncate()` and `mremap()` to resize storage. Mapping addresses may change, so raw pointers must not be retained across resizing operations.
- **clone()**: Uses `copy_file_range()` to copy files.

---

## 2. Mapping & Addressing

`FileManager` exposes a contiguous mapped address range. Other higher-level components should use logical byte offsets 
instead of absolute pointers.

> To learn more about logical storage allocation, see [DataStorage](../mem/data_storage.md).

---

## 3. Most important operations

- `create_blank()`: Creates and maps a backing file.
- `resize()`: Resizes the file and its mapping.
- `clone()`: Copies the backing file and maps the result.
- **Destruction:** Releases the owned mapping and file descriptor.
