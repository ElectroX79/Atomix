# I/O

> This is a proposed feature and its design is not finalized.

## Goal

The goal is to provide a simple interface for converting external tabular data
into `DataTable` objects and exporting them back to common formats.

```text
External data 
     │
     ▼
   Reader
     │
     ▼
   Parser ──────  Type information
     │         
     ▼
 DataTable
     |
     ▼
 Serializer
     |
     ▼
 File with a common format
     
     
 
 
```
## Current state

Atomix currently provides a low-level `Reader` for accessing files through
memory mapping.

`Reader` owns the file descriptor and the mapped memory region, providing a
simple read-only view over the file contents.

## Future direction

The I/O layer should initially support reading and writing CSV (just CSV for now) while keeping
format-specific parsing separate from the DataTable representation.

For now the type inference will be done manually, but in the future could be
automatically inferred from the file contents.