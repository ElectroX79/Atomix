# Persistence ram-disk (ADR-0007)

## Status
Pending test and benchmark

## Context
All the data is stored in memory (RAM). No file-backed storage was used.

## Decision
The data can be persisted in a file-backed storage, with a custom binary format.
When needed, the data can be loaded from the file-backed storage to the ram,
but just the metadata, the data is mapped with mmap();

The format is like the following diagram:

```text 

========================================================================================
                                    BITMASK STRUCTURE
========================================================================================

                 +-------------------+  +------------------+  +-------------------+
                 | vector<mask_t>    |  |                  |  |                   |
                 | size + data       |  | n_ones (8 bytes) |  | abs_size (8 bytes)|
                 | (8 + k bytes)     |  |                  |  |                   |
                 +---------+---------+  +--------+---------+  +---------+---------+
                           ^                     ^                      ^
                           |                     |                      |
                           +---------------------+----------------------+
                                                 |
                                       +---------+---------+
                                       |      BitMask      |
                                       | (8 + k + 8 + 8 B) |
                                       |   = (24 + k B)    |
                                       +-------------------+


========================================================================================
                                     MAIN LAYOUT
========================================================================================

+---------------+     +--------------------+     +-------------------+     +---------------------+     +-------------+
|   n_columns   | ==> |      metadata      | ==> | column_valid_mask | ==> |        data         | ==> |  valid_row  |
|   (8 bytes)   |     | [n* cols]impl.size |     | (BitMask=24+k B)  |     | [8(size)+n*ColData] |     |  (8 bytes)  |
+---------------+     +---------+----------+     +-------------------+     +----------+----------+     +-------------+
                                |                                                     |
                                |                                        +------------+------------+
                                v                                        v                         v
                      +-------------------+                    +-------------------+     +-------------------+
                      |      Column       |                    | chunks_n + chunks |     |     row_mask      |
                      | (8+k+2+2 = 12+k B)|                    |  (8 + n * ColData)|     | (BitMask=24+k B)  |
                      +---------+---------+                    +---------+---------+     +-------------------+
                                |                                        |
+-------------------------------v-------------------------------+        v
|                                                               |      +--------------------+
|  +---------------------------------------------------------+  |      | ColumnData (###)   |
|  |             name byte size + name data                  |  |      | (17 + 24+k + 17)   |
|  |                    (8 + k bytes)                        |  |      |    = (58 + k B)    |
|  +---------------------------------------------------------+  |      +---------+----------+
|                               |                               |                |
|                               v                               |       +--------+--------+
|  +---------------------------------------------------------+  |       |        |        |
|  |                 type, enum class                        |  |       v        v        v
|  |                     (2 bytes)                           |  |   +------+ +-------+ +------------------+
|  +---------------------------------------------------------+  |   | Data | |offsets| | nullmask         |
|                               |                               |   |(chunk| |(chunk | |(BitMask=24+k B)  |
|                               v                               |   |=17 B)| |=17 B) | +------------------+
|  +---------------------------------------------------------+  |   +---+--+ +---+---+
|  |              secondary type, enum class                 |  |       |        |
|  |                     (2 bytes)                           |  |       +---+----+
|  +---------------------------------------------------------+  |           |
|                                                               |           v
+---------------------------------------------------------------+   +-------------------+
                                                                    | Chunk (8+8+1=17 B)|
                                                                    +---------+---------+
                                                                              |
                                                          +-------------------+-------------------+
                                                          |                   |                   |
                                                          v                   v                   v
                                                  +---------------+   +---------------+   +---------------+
                                                  | offset (8 B)  |   |  size (8 B)   |   | reference(1B) |
                                                  +---------------+   +---------------+   +---------------+
```
## Motivation

- Persistence of the data in a file-backed storage.
- Avoiding the ram bottleneck.

## Consequences

### Advantages

- Ram capacity is no longer the main bottleneck.
- Data can be persisted in a file-backed storage.
- Fast intermediate binary format.

### Disadvantages
- More complex pages management.
- Need to implement a custom binary format.
- Possible I/O bottleneck.





