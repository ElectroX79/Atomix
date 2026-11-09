# Arquitecture of Atomix

Atomix is a data engine based mainly on the **data table** concept:

A **data table** is a data structure that represents a relational database table which 
in atomix has the following properties:

- Columnar layout
- Based by chunks/batches
- Chunks are persisted on disk

___


## 1. What is a columnar layout, and why is it important?
To understand what a columnar format is, consider the following table:

| id | name | age |
|----|-----|----|
| 1 | John | 30 |
| 2 | Mary | 25 |
| 3 | Bob | 40 |

There are several methods to store a data table, a usual one is stored as an array of rows:

```
+----------------+----------------+----------------+
|     Row 0      |     Row 1      |     Row 2      |
+----------------+----------------+----------------+
| 1 | John | 30  | 2 | Mary | 25  | 3 | Bob | 40  |
+----------------+----------------+----------------+

``` 
<br>
Other could be as an array of columns:

```
+--------------+-------------------+--------------+
| Column 0     | Column 1          | Column 2     |
| (id)         | (name)            | (age)        |
+--------------+-------------------+--------------+
| 1 | 2 | 3    | John | Mary | Bob | 30 | 25 | 40 |
+--------------+-------------------+--------------+
```

The first one is quite efficient for transactional systems where the data is modified locally by rows repeatedly throughout the time, but
not for analytical systems, which depend on columnar queries or operations. 

The main reason of why a proper layout is important is the contiguity, which allows efficient data movement and a better performance of some technologies, like:

- CPU caches.
- SIMD instructions.
- Multithreading.
- Etc...

Notice that both represent the same logical data structure, but each one has a different physical memory layout.
___

## 2. Why chunked batching?

Instead of allocating a monolithic buffer for an entire column, Atomix partitions 
columns into fixed-size chunks (e.g., 64 KiB).

This design serves two main purposes:
- **Cache locality and vectorization:** Chunks act as the atomic processing unit (a batch), 
fitting correctly in caches during SIMD operations.
- **Dynamic scaling:** Columns can grow incrementally without requiring massive reallocations or 
contiguous virtual memory mapping.

For architectural decisions about batch processing, see [ADR-0006](../adr/adr-0006-batching.md).

___

## 3. Persistence on disk

Atomix stores data directly in a chunk-serialized format on disk. Because memory chunks are already 
aligned and self-contained, serializing and deserializing data reduces translation overhead and 
avoids unnecessary copying.

For details on the disk storage format and I/O design, see [ADR-0007](../adr/adr-0007-persistence-ram-disk.md) and [File Manager Internals](../internals/io/file_manager.md).
___

To learn more about the implementation, go to [Data Table internals](../internals/data/data_table.md)