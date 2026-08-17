# DataTable

> **Before continuing, make sure you understand [Column](column.md) and the architecture of the [Data Table](../../architecture/data_table.md).**

A `DataTable` is a user-level storage abstraction in Atomix.

Its responsibility is to organize multiple `Column` instances into a single table while providing a consistent interface for inserting, accessing, and managing tabular data.

## Internal representation

Internally, a `DataTable` stores its columns in a `std::vector<Column>`. 

The reason is that `std::vector` handles the needs of `DataTable`, because it provides a standard
data structure to acces the data with an efficient sequential access and constant-time random acces.

## Responsibilities

`DataTable` is responsible for:

- Managing the lifetime of its columns.
- Maintaining table consistency.
- Providing the public API exposed to users.
- Coordinating operations across multiple columns.
