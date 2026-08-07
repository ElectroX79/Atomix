# Data table

A data table is a data structure that represents a relational database table which in atomix has the following properties:

- Columnar layout
- Designed to copy on write (CoW).
- Chunked storage.

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

## 2. About copying on write (COW)

Copy on write is an optimization technique that makes data table immutable and only can be modified by construction.

For instance, consider the following data table:
```
+--------------+-------------------+--------------+
| Column 0     | Column 1          | Column 2     |
| (id)         | (name)            | (age)        |
+--------------+-------------------+--------------+
| 1 | 2 | 3    | John | Mary | Bob | 30 | 25 | 40 |
+--------------+-------------------+--------------+
```
If you want to add +1 to each element of id, and you are using CoW, you cannot edit inplace, you have to create a new version of Column 0 and append it together.

To avoid copying each element every time we want to edit a table, we can store pointers of each column instead of the raw data, enabling shallow copying and allowing tables to share unchanged columns.

There are two reasons why this model is beneficial:

- That allows having two datatable with slight changes at the same time, but without having to worry about paying a high cost for the copy.
- Immutability greatly simplifies sharing data between threads, since readers can safely access the same data without requiring copies.

---
## 3. Why does it need chunking?

Copy-on-Write avoids modifying existing data, but copying an entire column is still expensive. For that reason we need to split the entire column
into smaller chunks which allow just copying the chunk which is being edited.

For example, instead of having: 1 column of 512 KB, it can be an array composed of 8 chunks of 64 KB. 
Using this example, if the first element is edited, without chunking it will copy 512 KB, but with chunking, 
it will just copy 64 KB.


___

To learn more about the implementation, go to [Data Table internals]()