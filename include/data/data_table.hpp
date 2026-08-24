#ifndef DATA_TABLE_HPP
#define DATA_TABLE_HPP

#include <vector>
#include <stdexcept>
#include <string_view>
#include <memory>
#include <iostream>
#include <cstdlib>


#include "data_type.hpp"
#include "mem/buffer.hpp"
#include "security_check.hpp"


namespace atomix {
    class DataTable{

#ifndef NDEBUG
        friend class DataTableTester;
#endif

        friend struct DataTablePrinter;

        struct ListMetadata{
            std::vector<uint32_t> offsets;
            uint32_t last_used_byte;
        };

        struct Column {
            std::string name;
            std::vector <mem::Buffer> buffers;
            std::vector<ListMetadata> list_metadata;
            size_t n_elements;
            size_t chunk_size; // 0 if it's not chunked (if chunk_size == 0 -> vector<shared_ptr<buffer>>.size() <= 1)
            DataType type = DataType::Undefined;
            DataType variable_type; //CANNOT BE LIST. Also, if type != DataType::List, then variable_type == DataType::Undefined

            Column(std::string&& name1,
                std::vector<mem::Buffer>&& buffer1,
                std::vector<ListMetadata>&& list_metadata1,
                const size_t n_elements1,
                const size_t chunk_size1,
                const DataType type1,
                const DataType variable_type1
                ):
            name(std::move(name1)),
            buffers(std::move(buffer1)),
            list_metadata(std::move(list_metadata1)),
            n_elements(n_elements1),
            chunk_size(chunk_size1),
            type(type1),
            variable_type(variable_type1)
            {}
        };



        std::vector<Column> columns_;


        //primitives methods


        /** @brief
         * * @param col_index indicates which column of columns_
         * * @param offset indicates the logical offset (not the physical, but the x element)
         * * @param buffer_index returned value to know which buffer access
         * * @param remainder returned value that represents the offset inside the buffer
         *
        **/
        void get_buffer_pos(const size_t col_index, const size_t offset, size_t& buffer_index, size_t& remainder)const{
            if (columns_[col_index].type != DataType::List) {
                const size_t chunk_size = columns_[col_index].chunk_size;
                const size_t byte_size = data_type_utils::byte_size_fixed(columns_[col_index].type);

                if (chunk_size == 0) {
                    buffer_index = 0;
                    remainder = offset* byte_size ;
                    return;
                }

                buffer_index = offset* byte_size  / chunk_size;
                remainder = offset* byte_size  % chunk_size;
            }
            else {
                throw std::logic_error("List support not implemented yet for get_buffer_pos()");
                //Implement
            }
        }

        template<DataType DT, DataType DT2 = DataType::Undefined> //if DT != DataType::List, then DT2 == DataType::Undefined
        void get_buffer_pos(const size_t col_index, const size_t offset, size_t& buffer_index, size_t& remainder)const{


            if constexpr(DT != DataType::List) {
                using T = type_of_t<DT>;
                const size_t chunk_size = columns_[col_index].chunk_size;

                if (chunk_size == 0) {
                    buffer_index = 0;
                    remainder = offset*sizeof(T);
                    return;
                }

                buffer_index = offset*sizeof(T) / chunk_size;
                remainder = offset*sizeof(T)  % chunk_size;
            }
            else {
                throw std::logic_error("List support not implemented yet for get_buffer_pos()");

                using T_aux [[maybe_unused]] = type_of_t<DT2> ; //variable_type, delete [[maybe_unused]] when implemented


            }
        }






    public:

        DataTable() = default;

        DataTable(const DataTable& other) = default;
        DataTable& operator=(const DataTable& other)noexcept = default;

        DataTable(DataTable&& other) = default;
        DataTable& operator=(DataTable&& other)noexcept{
            if (this != &other){
                columns_ = std::move(other.columns_);
            }
            return *this;
        }

        ~DataTable() = default;


        [[nodiscard]] size_t n_columns() const {
            return columns_.size();
        }

        [[nodiscard]] std::string_view column_name(const size_t index) const {
            atomix::bounds::check_index_individual(index, columns_.size());
            return columns_[index].name;
        }

        [[nodiscard]] DataType column_datatype(const size_t index) const {
            atomix::bounds::check_index_individual(index, columns_.size());
            return columns_[index].type;
        }

        [[nodiscard]] size_t column_size(const size_t index) const {
            atomix::bounds::check_index_individual(index, columns_.size());
            return columns_[index].n_elements;
        }

        /**
        * @brief Accesses an element by column index and logical offset.
        *
        * @tparam DT The column data type.
        * @param col_index The column index.
        * @param offset The logical element index.
        * @return The value at the specified position.
        *
        * @note `DataType::List` is not supported. Use `at_list()` instead.
        */
        template<DataType DT>
        [[nodiscard]] type_of_t<DT> at(const size_t col_index,const size_t offset)const{
            using T = type_of_t<DT>;

            bounds::check_index_individual(col_index, columns_.size());
            bounds::check_index_individual(offset, columns_[col_index].n_elements);

            if (DT != columns_[col_index].type) {
                std::cerr << "Contract violation: data type mismatch (DataTable::at)" << "\n";
                std::abort();
            }

            if constexpr (DT == DataType::List) {
                std::cerr << "DataTable do not accept DataType::List, use at_list instead " << "\n";
                std::abort();
            }

            size_t buffer_index, remainder;
            get_buffer_pos<DT>(col_index, offset, buffer_index, remainder);
            return *reinterpret_cast<T*>(columns_[col_index].buffers[buffer_index].get_begin() + remainder);
            //TODO: study using std::start_lifetime_as<T*> if it cannot assure lifetime
        }

        /**
        * @brief Accesses a list by column index and logical offset.
        *
        * @tparam DT The data type stored by the list, for example, a list of char (a pseudo string) DT == DataType::Char
        * @param col_index The column index.
        * @param offset The logical list index.
        * @return A span containing the list elements.
        *
        * @note The column must have `DataType::List` with `DT` as its variable type.
        */
        template<DataType DT>
       [[nodiscard]] std::span<type_of_t<DT>> at_list(const size_t col_index,const size_t offset)const {
            //TODO: do when implemented list support

            //You should implement firstly get_buffer_pos() for DataType::List, in that way this method would be easier to implement.

            //Notice that the method at() with DataType::List could have O(N) because get_buffer_pos(), where N is numbers of chunks cost without extra infrastructure,
            //that's expected for now.
            //Note: For sequential massive access is still O(1), depending of the scope.
            throw std::logic_error("List support not implemented yet for at()");
        }


        //TODO: implement std::forward for methods extract(), append() and erase
        /**
         * @brief Extract a copy of partial tabular data defined by an interval
         * * @param begin The first index of the interval of columns to extract
         * * @param end The last index of the interval of columns to extract
         * * @note Consider  [begin, end)
         */
        [[nodiscard]]DataTable extract(size_t begin, size_t end)const;

        /**
        * @brief Appends the content of another TabularData instance to the current one.
        * * Merges the data buffers of 't_data' into the current instance.
        * * @param t_data The source TabularData object to be copied and concatenated.
        * * @note Always appends at the end of the tabular data
        * * @note Always at the end of the tabular data
        */
        [[nodiscard]]DataTable append(const DataTable &t_data)const;

        /**
         * @brief Erase an interval of columns of the current tabular data
         * * @param begin The first index of the interval of columns to erase
         * * @param end The last index of the interval of columns to erase
         * * @note Consider [begin, end)
         */
        [[nodiscard]]DataTable erase(size_t begin, size_t end)const;






    };
}


#endif 