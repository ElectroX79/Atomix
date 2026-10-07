# Datatable structure layout refactor (ADR-0006)

## Status
Pending test and benchmark

## Context
Pure columnar data structure, more oop-oriented layout. No batching, just an infinite array in a column.

## Decision
Refactor the data table structure to be work with fixed-size batches in each batch:

For example, for a tabular table as:
```text
Table Data:
[ID: 1, 2, 3, 4]
[Age: 30, 25, 40, 22]
```


Before the physical layout was something like:
```text

Memory / Disk:
Col ID:  | 1 | 2 | 3 | 4 |
Col Age: | 30 | 25 | 40 | 22 |

```
- Each column is an independent contiguous block of memory.

After the refactor it seems like:
```text
Memory / Disk:

[ BATCH 1 (Rows 1-2) ]
  Col ID:  | 1 | 2 |
  Col Age: | 30 | 25 |

[ BATCH 2 (Rows 3-4) ]
  Col ID:  | 3 | 4 |
  Col Age: | 40 | 22 |

```
As can be seen, the structure maintains locally the columnar layout, but now it is organized in batches.


## Motivation

- Preparation for ram-disk support.
- More suitable as a base to implement complex operations.

## Consequences

### Advantages

- Different columns are now unified by rows.
- Easier erasing and inserting rows.
- Possibility of multiple structural optimizations like bloom filters, selection vector 
or data skipping.
- Better performance for operations that require sequential access or sql-like queries.
- Easier parallelism.

### Disadvantages
- More complex data structure.
- More complex metadata.
- Require strong contracts and invariants.




