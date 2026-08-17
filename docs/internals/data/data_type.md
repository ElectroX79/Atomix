# DataType

`DataType` is an enumeration that represents all data types currently supported by Atomix.

It provides a common runtime representation for column types and allows the engine to reason about data independently of C++ template types.

---

## Current implementation

`DataType` currently defines the following values:

- `Int32`
- `Float64`
- `Bool`
- `List`
- `Undefined` *(used as the default value)*

Additionally, Atomix provides helper functions to:

- Retrieve the size of a data type in bytes.
- Convert a `DataType` value to its string representation.

---
## What is a `List`?

List is a special datatype that defines a data type that has a variable length.

Usually need extra external format support on the container that accepts lists, like `Column`

---

## Why doesn't Atomix use C++ types directly?

There are two main reasons.

- Atomix needs a runtime representation of a type that can be stored as ordinary data.

- Not every logical data type has the same implementation as its C++ counterpart. For example, `List` requires a dedicated storage strategy because it has a variable size.
---

## Does this sacrifice the metaprogramming?

No.

Internally, Atomix uses C++ type traits to map C++ types to `DataType` values whenever possible.

However, you should avoid relying directly on C++ type properties when implementing engine features. Instead, they should use the helper functions provided by `DataType`, as some logical data types may have specialized implementations.