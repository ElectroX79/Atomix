module;

#include <vector>
#include <stdexcept>
#include <string_view>
#include <memory>
#include <iostream>
#include <cstdlib>


export module atomix.data.data_table:core;
import atomix.data.data_type;
import atomix.mem;
import atomix.bounds;
import atomix.config;


namespace atomix::io {
    class ParserCsv;
}

export namespace atomix {



    class DataTable{

#ifndef NDEBUG
        friend class DataTableTester;
#endif
        //TODO: Consider creating a proxy accessor instead of friend class/struct
        friend struct DataTablePrinter;
        friend class io::ParserCsv;

        struct ListMetadata{
            std::vector<uint32_t> offsets = {};
            uint32_t last_used_byte;
        };

        struct Column {
            std::string name;
            std::vector<mem::Buffer> buffers;
            std::vector<ListMetadata> list_metadata;
            size_t n_elements;
            DataType type = DataType::Undefined;
            DataType variable_type; //CANNOT BE LIST. Also, if type != DataType::List, then variable_type == DataType::Undefined


            static std::vector<mem::Buffer> create_buffers(const size_t byte_size){
                std::vector<mem::Buffer> buffers1;
                buffers1.reserve((byte_size / (mem::chunk_size)) + 1);

                const size_t remainder = (byte_size % (mem::chunk_size));

                for (size_t i = 0; i < byte_size / (mem::chunk_size); ++i) {
                    buffers1.emplace_back(mem::chunk_size, mem::default_alignment);
                }
                if (remainder != 0){
                    buffers1.emplace_back(remainder, mem::default_alignment);
                }
                return buffers1;
            }


            template <DataType DT1, DataType DT2 = DataType::Undefined>
            static Column create_column(std::string&& name1,const size_t byte_size){
                Column column;
                column.buffers = create_buffers(byte_size);

                if constexpr (DT1 == DataType::List && DT2 == DataType::Undefined) {
                    std::cerr << "Contract violation: DataType::List requires a variable type" << "\n";
                    std::abort();
                }

                if constexpr (DT1 != DataType::List && DT2 != DataType::Undefined) {
                    std::cerr << "Contract violation: Only dataType::List can have a variable type" << "\n";
                    std::abort();
                }

                if constexpr (DT1 != DataType::List) {
                    column.name = std::move(name1);
                    column.type = DT1;
                    column.variable_type = DataType::Undefined;
                    column.n_elements = byte_size/data_type_utils::byte_size_fixed(DT1);
                    column.list_metadata.clear();

                }
                else {
                    column.name = std::move(name1);
                    column.type = DT1;
                    column.variable_type = DT2;
                    column.n_elements = 0;// undefined, define manually
                    column.list_metadata = std::vector<ListMetadata>(column.buffers.size()); //it needs to be initialized
                }

                return column;
            }

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
                const size_t byte_size = data_type_utils::byte_size_fixed(columns_[col_index].type);


                buffer_index = offset* byte_size  / mem::chunk_size;
                remainder = offset* byte_size  % mem::chunk_size;
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


                buffer_index = offset*sizeof(T) / mem::chunk_size;
                remainder = offset*sizeof(T)  % mem::chunk_size;
            }
            else {
                throw std::logic_error("List support not implemented yet for get_buffer_pos()");
                using T_aux [[maybe_unused]] = type_of_t<DT2> ; //variable_type, delete [[maybe_unused]] when implemented
                //Implement

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
        void append(const DataTable &t_data);

        /**
         * @brief Erase an interval of columns of the current tabular data
         * * @param begin The first index of the interval of columns to erase
         * * @param end The last index of the interval of columns to erase
         * * @note Consider [begin, end)
         */
        void erase(size_t begin, size_t end);






    };
}


