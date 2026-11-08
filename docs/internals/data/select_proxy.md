# SelectProxy

This class is used as a select mask por datatable without any copy.

### Components
This class has the following components:
- `std::vector<BitMask>` : Used to filter the desired rows of the `DataTable`.
- `BitMask column_mask_`: Used to filter the desired columns of the `DataTable`.
- `DataTable* table_reference_`: The desired `DataTable` to be filtered.

## Features
- Can be constructed with a `DataTable`.
- A method that prints the content of the DataTable under the `SelectProxy` mask.
- Another function to sum all the values of a specific column.
