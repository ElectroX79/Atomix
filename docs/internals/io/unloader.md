# Unloader

The `unloader` handles the binary serialization and persistence of an active `DataTable` to persistent storage (disk).

It persists the full runtime state into the table's paired binary files:
- `.atomix`: Contains table metadata, batch structures, bitmasks, and fixed-size columnar data.
- `.atomixvar`: An append-only storage arena dedicated to variable-length payloads (such as strings or dynamic lists).


---

## Main Responsibilities

### 1. Pre-Allocation Sizing (`calculate_metadata_size()`)
Calculates the exact byte footprint needed for the entire table footer before writing to disk.

### 2. Saving Tables to Disk (`save_file()`)
Writes the active table state and footer metadata into persistent storage in a single sequential pass, doing the following:
- Updates main file headers with the current table state.
- Serializes the metadata, to learn more about metadata, go to [Metadata](../data/metadata.md).
- Writes column-level and batch-level validity bitmasks with total valid row counts.
- Stores all the data for every batch.
- Updates headers on the paired `.atomixvar` file to keep variable-length storage synchronized with the table state.