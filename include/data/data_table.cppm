module;

#include <vector>
#include <stdexcept>
#include <string_view>
#include <memory>
#include <iostream>
#include <variant>
#include <cstring>


export module atomix.data.data_table;
import atomix.data.data_type;
import atomix.mem;
import atomix.bounds;
import atomix.config;
import atomix.mem.data_storage;
import atomix.bit_mask;
import atomix.data.metadata_data_table;
import atomix.mem.unloader;
import atomix.load_unaligned;



export namespace atomix::data {

    class DataTable;

    namespace detail {
        [[nodiscard]] inline std::vector<Column>& unsafe_get_metadata(DataTable& dt);
        [[nodiscard]] inline std::vector<DataBatch>& unsafe_get_data(DataTable& dt);
        [[nodiscard]]inline mem::DataStorage& unsafe_get_storage(DataTable& dt);
        [[nodiscard]]inline mem::DataStorage& unsafe_get_variable_storage(DataTable& dt);
        [[nodiscard]]inline BitMask& unsafe_get_column_valid_mask(DataTable& dt);
        [[nodiscard]]inline size_t& unsafe_get_valid_rows_(DataTable& dt);
    }



    class DataTable{

        friend std::vector<Column>& detail::unsafe_get_metadata(DataTable&);
        friend std::vector<DataBatch>& detail::unsafe_get_data(DataTable&);
        friend mem::DataStorage& detail::unsafe_get_storage(DataTable&);
        friend mem::DataStorage& detail::unsafe_get_variable_storage(DataTable&);
        friend BitMask& detail::unsafe_get_column_valid_mask(DataTable& dt);
        friend size_t& detail::unsafe_get_valid_rows_(DataTable& dt);


        std::vector<Column> metadata_;
        BitMask column_valid_mask_;

        std::vector<DataBatch> data_;
        size_t valid_rows_ = 0;


        mem::DataStorage storage_;
        mem::DataStorage variable_storage_;


        //primitives methods



        void allocate_batch() {
            data_.emplace_back();
            data_.back().row_mask =  BitMask(n_rows_batch, false);

            for (const auto& column:metadata_) {
                data_.back().chunks.emplace_back();

                if (column.type == DataType::List) {

                    //the caller has to instance data_.back().chunks.back().data
                    data_.back().chunks.back().offsets =
                        storage_.allocate_chunk(n_rows_batch * sizeof(ColumnData::offset_t));
                }

                else {
                    data_.back().chunks.back().data =
                        storage_.allocate_chunk(n_rows_batch * data_type_utils::byte_size_fixed(column.type));
                    // data_.back().chunks.back().offsets = default (none)
                }
            }
        }




        //pos: you need to manually allocate the needed space for variable_storage_

        Position reserve_rows(const size_t size) {

            //0- create initial size
            const Position pos(data_.size(), 0);
            //-0


            //1- calculate n_batches and reserve for data_:
            size_t n_batches = size / n_rows_batch;
            if (size % n_rows_batch != 0 ){
                n_batches++;
            }

            data_.reserve(data_.size() + n_batches);
            //-1

            //2-calculate byte_size_row and reserve enough in storage
            size_t byte_size_row = 0;
            for (const auto& column:metadata_) {
                if (column.type == DataType::List)
                    byte_size_row += sizeof(ColumnData::offset_t);
                else
                    byte_size_row += data_type_utils::byte_size_fixed(column.type);
            }
            storage_.reserve(storage_.data_size() + n_batches * n_rows_batch * byte_size_row);
            //-2

            //3-allocate storage and set data_ metadata
            for (size_t i = 0; i < n_batches; ++i) {
                allocate_batch();
            }
            //-3


            return pos;
        }


        /** @brief
         * * @param col_index indicates which column of columns_
         * * @param offset indicates the logical offset (not the physical, but the x element)
         * * @param data_index returned value to know which buffer access
         * * @param internal_index returned value that represents the offset inside the buffer
         *
        **/

        /* deprecated
        void get_buffer_pos(const size_t col_index, const size_t offset, size_t& data_index, size_t& internal_index)const{
            if (columns_[col_index].type != DataType::List) {

                const size_t byte_size = data_type_utils::byte_size_fixed(columns_[col_index].type);

                data_index = offset/data::n_rows_batch;

                internal_index = d
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
        */





    public:

        DataTable() = default;

        //instead of copy operator and constructor, use DataTable::clone()
        DataTable(const DataTable& other) = delete;
        DataTable& operator=(const DataTable& other) = delete;

        DataTable(DataTable&& other) = default;
        DataTable& operator=(DataTable&& other) = default;
        ~DataTable() {
            mem::unloader::save_file(metadata_,
                                    column_valid_mask_,
                                    data_,
                                    valid_rows_,
                                    storage_,
                                    variable_storage_
                                    );
        }


        [[nodiscard]]DataTable clone(const std::string& file_name)const{
            DataTable other;
            other.metadata_ = metadata_;
            for (const auto& batch : data_) {
                other.data_.emplace_back(batch.unsafe_clone());
            }
            
            other.column_valid_mask_ = column_valid_mask_;
            other.valid_rows_ = valid_rows_;

            other.storage_ =
                storage_.clone(file_name);
            other.variable_storage_ =
                variable_storage_.clone(file_name + "var");

            return other;
        };





        [[nodiscard]] size_t n_columns() const {
            return metadata_.size();
        }

        [[nodiscard]] std::string_view column_name(const size_t index) const {
            atomix::bounds::check_index_individual(index, metadata_.size());
            return metadata_[index].name;
        }

        [[nodiscard]] DataType column_datatype(const size_t index) const {
            atomix::bounds::check_index_individual(index, metadata_.size());
            return metadata_[index].type;
        }



        void set_new_column(const std::string& name, const DataType type, const DataType second_type = DataType::Undefined) {
            if (!data_.empty())
                throw std::logic_error("Cannot add new column to a DataTable with data, please use append_column() instead");

            column_valid_mask_.push_back(true);
            metadata_.emplace_back(name, type, second_type);
        }

        /*
        void append_column(const std::string& name,
                            const TGroupColumn& col_data,
                            const BitMask& col_null_mask) {

            auto get_size = [](const auto& column) -> std::size_t {
                return column.size();
            };
            const size_t col_data_size = std::visit(get_size, col_data);

            if (col_data_size != col_null_mask.total_bits()) {
                throw std::invalid_argument
                    ("Contract broken: number of elements does not match to number of nulls of the table");
            }

            if (col_data_size != valid_rows_) {
                throw std::invalid_argument
                    ("Contract broken: number of elements does not match to numbers of row of the table");
            }



            auto function = [&]<TDataTypeCompatibleCol T>(T&& value) -> void {
                const CompDataType CDT =  type_to_datatype_col<std::remove_cvref_t<T>>();

                if (CDT.type != DataType::List) {
                    size_t n_batch = 0; //index for data_ (data_[n_batch]);
                    size_t n_element = 0; //index for col_data and col_null_mask (x[n_element]);
                    while(n_batch < data_.size() && n_element < col_data_size) {
                        data_[n_batch].chunks.emplace_back();
                        data_[n_batch].chunks.back().data =
                            storage_.allocate_chunk(data_type_utils::byte_size_fixed(CDT.type) * n_rows_batch);
                        data_[n_batch].chunks.back().null_mask = BitMask(n_rows_batch, true);

                        for (size_t i = 0; i < n_rows_batch; ++i) {
                            data_[n_batch].row_mask[i];
                        }

                        n_element++;
                        n_batch++;

                    }
                }
                else {
                    throw std::logic_error("List support not implemented yet for append_column()");
                }

            };
            std::visit(col_data, function);


        }
        /*


        //TODO: finish add_row
        /*
        void add_rows(const std::vector<std::vector<TGroup>&>& v_data) {
            Position pos = reserve_rows(v_data.size());

            if (v_data.empty()) {
                return;
            }




            //1 major loop
            for (size_t i = 0; i < v_data.size(); ++i) {
                if (v_data[i].size() != metadata_.size()) {
                    throw std::invalid_argument("Contract broken: columns count does not match");
                }
                for (size_t j = 0; j < v_data[i].size(); ++j) {
                    auto function = [&]<TDataTypeCompatible T>(T&& value) {
                        constexpr DataType DT = type_to_datatype<T>().type;
                        if (DT != metadata_[j].type)
                            throw std::logic_error("Contract broken: data type does not match");

                        if constexpr (DT_is_fixed_t<DT>){

                            const auto& chunk = data_[pos.batch_index].chunks[j].data;

                            std::byte* ptr =
                                chunk.calculate_ptr(
                                    pos.row_index + i,
                                    storage_.begin(),
                                    sizeof(T)
                                );

                            store_unaligned(ptr, std::forward<T>(value));
                        }
                        else {
                            const auto& chunk_offsets = data_[pos.batch_index].chunks[j].offsets;
                            uint64_t prev_uint64 = 0

                            if (i != 0)[[likely]] {
                                std::byte* ptr_prev =
                                    chunk_offsets.calculate_ptr(
                                        pos.row_index + i - 1,
                                        storage_.begin(),
                                        sizeof(uint64_t)
                                    );

                                const auto prev_ptr_uint64 =
                                    std::start_lifetime_as<uint64_t>(ptr_prev);

                                prev_uint64 = *prev_ptr_uint64
                            }


                            std::byte* ptr =
                                chunk_offsets.calculate_ptr(
                                    pos.row_index + i,
                                    storage_.begin(),
                                    sizeof(uint64_t)
                                );

                            store_unaligned(ptr , value.size() + prev_uint64 );
                        }

                    }

                    std::visit(function, v_data[i][j]);

                }
            }

            //2 major loop
            for (size_t j = 0; j < v_data[0].size; ++j) {
                if (v_data[0][j].type == DataType::List) {
                    uint64_t storage_needed = 0;
                    for (size_t i = 0; i < v_data.size(); ++i) {

                        auto function = [&]<TDataTypeCompatible T>(T&& value) {
                            constexpr DataType DT = type_to_datatype<T>().type;
                            static_assert(DT == DataType::List, "Program internal contract violated, please report issue");

                            storage_needed += value.size();

                        }

                        std::visit(function, v_data[i][j]);
                    }
                    // TODO: using storage_needed to reserve and after reserve, do a second loop ( rn doing this)
                    //reserving_variable_storage() ... TASK DONE
                    variable_storage_.reserve(storage_needed);

                    for (size_t i = 0; i < v_data.size(); ++i) {

                        auto function = [&]<TDataTypeCompatible T>(T&& value) {
                           //store the information in the chunks TASK DOING
                            v_data[i][j]
                        }

                        std::visit(function, v_data[i][j]);
                    }
                }
            }


        }
        */
        void add_batch(const std::vector<TGroupColumn>& v_data) {

            if (v_data.empty()) {
                return;
            }

            auto get_size = [](const auto& column) -> std::size_t {
                return column.size();
            };

            const size_t size_ref = std::visit(get_size, v_data[0]);
            for (const auto& column:v_data) {
                if (std::visit(get_size, column) != size_ref ) {
                    throw std::invalid_argument("Contract broken: columns count does not match");
                }
            }

            if (size_ref == 0) {
                return;
            }

            auto get_ptr = []<TDataTypeCompatibleCol T>(const T& column) -> std::byte* {
                return reinterpret_cast<std::byte*>(column.data());
            };


            const Position pos = reserve_rows(std::visit(get_size, v_data[0]));
            const Position upper_bound = pos + std::visit(get_size, v_data[0]);
            const size_t diff = upper_bound.to_abs() - pos.to_abs();

            // Fill table-level row_mask across batch boundaries
            {
                Position curr_pos = pos;
                size_t rows_processed = 0;
                const size_t total_rows = diff;

                while (rows_processed < diff) {
                    const size_t avail_in_batch = n_rows_batch - curr_pos.row_index;
                    const size_t amount = std::min(total_rows - rows_processed, avail_in_batch);


                    data_[curr_pos.batch_index].row_mask.set_range(
                        BitMask::to_bit_pos(curr_pos.row_index),
                        BitMask::to_bit_pos(curr_pos.row_index + amount),
                        true
                    );
                    valid_rows_ += amount;


                    rows_processed += amount;
                    curr_pos.batch_index++;
                    curr_pos.row_index = 0;
                }
            }


            for (size_t i = 0; i < v_data.size(); ++i) {
                auto function = [&]<TDataTypeCompatibleCol T>(T&& value) -> void {
                    constexpr CompDataType CDT =  type_to_datatype_col<std::remove_cvref_t<T>>();
                    if (CDT.type != metadata_[i].type) {
                        throw std::logic_error(
                            "Contract broken: data type does not match");
                    }


                    Position curr_pos = pos;
                    size_t elem_copied = 0;
                    const size_t total_elem = diff;
                    auto sp_ptr = get_ptr(value);

                    if constexpr(CDT.type != DataType::List) {
                        using raw_t = decltype(value[0]);

                        while (elem_copied < diff) {

                            const size_t avail_in_batch = n_rows_batch - curr_pos.row_index;
                            const size_t amount = std::min(total_elem - elem_copied, avail_in_batch);


                            data_[curr_pos.batch_index].chunks[i].null_mask.set_range(
                                BitMask::to_bit_pos(curr_pos.row_index),
                                BitMask::to_bit_pos(curr_pos.row_index + amount),
                                true
                            );

                            auto src =  sp_ptr + elem_copied * sizeof(raw_t);
                            auto dest =
                                data_[curr_pos.batch_index].chunks[i].data.calculate_ptr(
                                    curr_pos.row_index, storage_.begin(), sizeof(raw_t)
                                    );
                            memcpy(dest, src, amount * sizeof(raw_t));

                            elem_copied += amount;
                            curr_pos.batch_index++;
                            curr_pos.row_index = 0;
                        }
                    }
                    else{
                        throw std::logic_error("List support not implemented yet for add_batch()");
                    }
                };

                std::visit<void>(function, v_data[i]);
            }
        }
        



    };


    namespace detail {
        inline std::vector<Column>& unsafe_get_metadata(DataTable& dt){
            return dt.metadata_;
        }
        inline std::vector<DataBatch>& unsafe_get_data(DataTable& dt) {
            return dt.data_;
        }
        inline mem::DataStorage& unsafe_get_storage(DataTable& dt) {
            return dt.storage_;
        }
        inline mem::DataStorage& unsafe_get_variable_storage(DataTable& dt){
            return dt.variable_storage_;
        }
        [[nodiscard]]inline BitMask& unsafe_get_column_valid_mask(DataTable& dt) {
            return dt.column_valid_mask_;
        }
        [[nodiscard]]inline size_t& unsafe_get_valid_rows_(DataTable& dt) {
            return dt.valid_rows_;
        }
    }

}


/*
if (pos.batch_index > 0) {
                            if  (n_rows_batch - pos.row_index > remainder_elem) {

                                auto src =
                                    get_ptr(value) + pos.row_index * sizeof(T);
                                auto dest =
                                    data_[pos.batch_index].chunks[i].data.calculate_ptr(pos.row_index, storage_.begin(), sizeof(T));
                                memcpy(src, dest, remainder_elem * sizeof(T));
                            }
                            else {
                                auto src =
                                    get_ptr(value) + pos.row_index * sizeof(T);
                                auto dest =
                                    data_[pos.batch_index].chunks[i].data.calculate_ptr(pos.row_index, storage_.begin(), sizeof(T));

                                memcpy(src, dest, n_rows_batch - pos.row_index * sizeof(T));
                                remainder_elem -= n_rows_batch - pos.row_index;
                            }
                            loop_offset2 = 1;
                        }


                        for (size_t j = loop_offset2; j < upper_bound.batch_index; ++j) {

                            auto src =
                                    get_ptr(value) + pos.row_index * sizeof(T);
                            auto dest =
                                data_[pos.batch_index].chunks[j].data.calculate_ptr(0 , storage_.begin(), sizeof(T));

                            memcpy(src, dest, n_rows_batch * sizeof(T));
                            remainder_elem -= n_rows_batch;
                        }

                        auto src =
                                    get_ptr(value) + pos.row_index * sizeof(T);
                        auto dest =
                            data_[pos.batch_index].chunks[i].data.calculate_ptr(0 , storage_.begin(), sizeof(T));
                        memcpy(src, dest, upper_bound.row_index * sizeof(T));
                        remainder_elem -= upper_bound.row_index;


                        if (remainder_elem != 0) {
                            std::cerr << "ERROR: remainder_elem != 0, internal error, please report" << "\n";
                            std::abort();
                        }
 */