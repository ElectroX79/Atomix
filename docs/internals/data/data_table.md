# DataTable

`DataTable` is the main columnar table abstraction in Atomix. It provides a tabular interface while managing schema metadata, validity bitmasks, batches, and physical memory.

---

## 1. Design & Lifecycle

- **Move-Only Ownership:** Copy operations are deleted to prevent accidental memory copies.
To duplicate data, an explicit deep `clone()` should be used.

- **Automatic Persistence:** With RAII, the table automatically serializes its metadata, validity masks, 
and data buffers to disk through an unloader component when the destructor is called.

---

## 2. Internal Layout

Data tables are composed of three main groups of components:

- **Schema and bitmasks:** Describes the column types, names, and validity state.

- **Batch partitioning:** Rows are split into fixed-size batches. This keeps execution units small enough to 
fit in caches and provides a better way to parallelize workloads.

- **Split physical storage:**
    * **Fixed-size storage:** Stores primitive numeric types (integers, floats) with 64-byte alignment.
    * **Variable-size storage:** An append-only arena for dynamic payloads (like strings). 

>To learn more about the internal layout, see [Metadata](metadata.md).

---

## 3. Ingestion workflow
To ingest data into a table, before writing rows, the table calculates the required number of batches and exact byte size. After this,
it reserves memory in a single step to avoid costly reallocations.

Ingestion copies data column by column directly into the table's storage.

The last step would be setting the row visibility and validity bitmasks.

---
### References
- Go to [Data Types](data_type.md) to learn more about the different data types supported by Atomix.