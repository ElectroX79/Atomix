# Loader

The `loader` module handles the binary initialization and deserialization of a `DataTable` from persistent storage (disk).

It operates on two paired binary files for each table:
- `.atomix`: Contains table metadata, batch structures, bitmasks, and fixed-size columnar data.
- `.atomixvar`: An append-only storage arena dedicated to variable-length payloads (such as strings or dynamic lists).

---

## Main Responsibilities

### 1. Creating Blank Tables (`initialize_table_clean()`)
Initializes a new and empty `DataTable` backed by clean binary files:
- Formats file headers for both files with format version tags and default alignment padding. 
- Configures base offsets for data and footer metadata.
- Prepares empty backing storage containers (`DataStorage`) linked to the new files.

### 2. Restoring Tables from Disk (`initialize_table()`)
Reconstructs an active `DataTable` object from existing binary files, doing the following:
- Reads file format versions, column counts, schema types, and column names from the file footer.
- Reconstructs table-level and batch-level validity bitmasks, batch partitions, and row validity counts.
- Rebuilds chunk references and offset vectors to map memory locations without moving underlying data.
- Construct both the primary and variable file managers into `DataStorage` instances.