# Metadata

Metadata describes the logical schema and the physical layout of the data inside Atomix.

It is divided into two main categories:

- **Column Descriptors:** The definition of each column in the table.
- **Batch Descriptors:** Data regions, chunk boundaries, and validity masks.

---

## 1. Column Descriptors

A column descriptor defines the identity and data type of an individual column:

* **Name:** Identifies the column across queries and table operations.
* **Primary Type:** Specifies the base data type (such as integers, floats, or lists).
* **Secondary Type:** Used exclusively when the primary type is a list. It defines the type of elements stored inside the list; otherwise, it remains undefined.

---

## 2. Batch Descriptors

Data in Atomix is processed in fixed-size blocks called batches. Batch descriptors define the memory locations, offsets, and row visibility.

### ColumnData
Each column inside a batch has its own descriptor that manages:

- **Fixed Data Reference:** Points to the memory chunk storing fixed-size values.
- **Offset Reference:** Points to the index offsets needed to read variable-length data (such as list elements).
- **Element Validity Mask:** A compact bitmask that tracks whether individual column values are valid or null.


### Batch of data
Combines the column chunks into an execution unit:

- **Column Collection:** Groups the differnts ColumnData descriptors into a single collection.
- **Row Active Mask:** A bitmask that stores which rows in the batch are currently active and visible.

___

## Related Topics

- Go to [File manager](../io/file_manager.md) or [Data Storage](../mem/data_storage.md) to learn 
more about how data is stored in Atomix.

- Go to [Loader](../io/loader.md) to explore how data is deserialized and loaded into Atomix, or read
[Unloader](../io/unloader.md) to know how Atomix does to the opposite of the loader.

