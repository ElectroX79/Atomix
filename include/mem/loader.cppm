module;
#include <cassert>
#include <cstdint>
#include <cstring>
#include <functional>
#include <cstddef>
#include <stdexcept>
#include <string>

export module atomix.mem.loader;
import atomix.data.data_table;
import atomix.data.data_type;
import atomix.mem;
import atomix.config;
import atomix.mem.chunk;
import atomix.load_unaligned;
import atomix.mem.file_manager;
import atomix.mem.data_storage;
import atomix.bit_mask;
import atomix.data.metadata_data_table;
import atomix.bounds;


namespace atomix::mem {

        size_t read_string(const std::byte* begin, std::string& str, const size_t pointer, const size_t container_size){
            bounds::check_index_individual(pointer, container_size);
            using type_parse = uint64_t;
            const auto size =
                load_unaligned<type_parse>(begin + pointer);

            bounds::check_offset_length(pointer + sizeof(type_parse), size , container_size);

            str.resize(size);

            memcpy(str.data(),
                begin + pointer + sizeof(type_parse),
                size
            );
            return pointer + size + sizeof(type_parse);
        }

        void read_bit_mask(const std::byte* begin_, size_t& pointer, BitMask& bit_mask, const size_t container_size) {
            bounds::check_index_individual(pointer, container_size);
            const auto bit_mask_size =
                load_unaligned<uint64_t>(begin_ + pointer);
            pointer += sizeof(uint64_t);

            constexpr size_t extra_bytes = 2 * sizeof(uint64_t);
            if (pointer > container_size || extra_bytes > (container_size - pointer)) {
                throw std::out_of_range("Container truncated in bitmask header");
            }
            const size_t available_capacity = (container_size - pointer) - extra_bytes;
            if (bit_mask_size > available_capacity / sizeof(mask_t)) {
                throw std::out_of_range("bit_mask_size exceeds file buffer or causes overflow");
            }

            const size_t bytes_to_read = bit_mask_size * sizeof(mask_t) + extra_bytes;


            bounds::check_offset_length(pointer, bytes_to_read, container_size);

            bit_mask.unsafe_get_mask().resize(bit_mask_size);
            //remember, although BitMask being bit-oriented container, when deserializing,
            //we treat it as a vector<mask_t>

            memcpy(bit_mask.unsafe_get_mask().data(),
                 begin_ + pointer,
                   bit_mask_size * sizeof(mask_t)
                  );
            pointer += bit_mask_size * sizeof(mask_t);

            bit_mask.unsafe_get_n_ones() =
                load_unaligned<uint64_t>(begin_ + pointer);
            pointer += sizeof(uint64_t);

            bit_mask.unsafe_get_abs_size() =
                load_unaligned<uint64_t>(begin_ + pointer);
            pointer += sizeof(uint64_t);
        }
    }

export namespace atomix::mem::loader{


    [[nodiscard]] data::DataTable initialize_table_clean(const std::string& file_name) {
        FileManager main_manager = FileManager::create_blank(file_name, atomix_format_main_header_size);
        FileManager variable_manager = FileManager::create_blank(file_name + "var", atomix_format_var_header_size);

        data::DataTable dt;
        auto& metadata = data::detail::unsafe_get_metadata(dt);
        auto& data = data::detail::unsafe_get_data(dt);
        auto& storage = data::detail::unsafe_get_storage(dt);
        auto& variable_storage = data::detail::unsafe_get_variable_storage(dt);
        auto& column_valid_mask = data::detail::unsafe_get_column_valid_mask(dt);
        auto& valid_rows = data::detail::unsafe_get_valid_rows_(dt);

        metadata.clear();
        data.clear();
        column_valid_mask.clear();
        valid_rows = 0;


        std::byte* begin_main = main_manager.begin();
        size_t pointer = 0;

        store_unaligned<uint64_t>(begin_main + pointer, atomix_format_version); //version
        pointer += sizeof(uint64_t);

        store_unaligned<uint64_t>(begin_main + pointer, atomix_format_main_header_size); //valid_data_end_offset
        pointer += sizeof(uint64_t);

        store_unaligned<uint64_t>(begin_main + pointer, atomix_format_main_header_size); //footer_metadata_offset
        pointer += sizeof(uint64_t);


        assert(pointer <= atomix_format_main_header_size && "Container truncated in header");

        memset( begin_main + pointer, 0, atomix_format_main_header_size - pointer);
        //end of the header config 24 bytes of header + 64 bytes padding/empty for future metadata


        storage =
            DataStorage (std::move(main_manager),
                     atomix_format_main_header_size,
                             PtrReference::Main,
                    atomix_format_main_header_size
            );

        std::byte* begin_var = variable_manager.begin();
        size_t pointer_var = 0;

        store_unaligned<uint64_t>(begin_var + pointer_var, atomix_format_version); //version
        pointer_var += sizeof(uint64_t);
        store_unaligned<uint64_t>(begin_var + pointer_var, atomix_format_var_header_size);//valid_data_end_offset
        pointer_var += sizeof(uint64_t);

        assert(pointer_var <= atomix_format_var_header_size && "Container truncated in header");
        memset( begin_var + pointer_var, 0, atomix_format_var_header_size - pointer_var);

        //end of the variable file header config approx 16 bytes of header + 64 bytes padding/empty for future metadata
        variable_storage =
            DataStorage (std::move(variable_manager),
                                atomix_format_var_header_size,
                                 PtrReference::Variable,
                                 atomix_format_var_header_size
            );


        return dt;
    }


     [[nodiscard]] data::DataTable initialize_table( const std::string& main_manager_name, const std::string& variable_manager_name){

        FileManager main_manager(main_manager_name);
        FileManager variable_manager(variable_manager_name);


        const std::byte* begin_ = main_manager.begin();
        data::DataTable dt;
        auto& metadata = data::detail::unsafe_get_metadata(dt);
        auto& data = data::detail::unsafe_get_data(dt);
        auto& storage = data::detail::unsafe_get_storage(dt);
        auto& variable_storage = data::detail::unsafe_get_variable_storage(dt);
        auto& column_valid_mask = data::detail::unsafe_get_column_valid_mask(dt);
        auto& valid_rows = data::detail::unsafe_get_valid_rows_(dt);

        size_t pointer = 0;

        const auto version =
            load_unaligned<std::uint64_t>(begin_ + pointer);
        pointer += sizeof(std::uint64_t);

        const auto data_size =
            load_unaligned<std::uint64_t>(begin_ + pointer);
        pointer += sizeof(std::uint64_t);

        const auto capacity = main_manager.size();

        const auto metadata_offset =
            load_unaligned<uint64_t>(begin_ + pointer);


        pointer += sizeof(uint64_t);
        pointer = metadata_offset;
        if (metadata_offset > capacity || metadata_offset <= atomix_format_main_header_size ) {
            throw std::out_of_range("metadata_offset is invalid");
        }
        // ### 2 -
        const auto n_columns =
            load_unaligned<uint64_t>(begin_ + pointer);
        pointer += sizeof(uint64_t);


        metadata.resize(n_columns);
        for (size_t i = 0; i < n_columns; ++i) {

            pointer = read_string(begin_, metadata[i].name, pointer,  capacity);

            metadata[i].type =
                static_cast<data::DataType>(load_unaligned<uint16_t>(begin_ + pointer));
            pointer += sizeof(uint16_t);

            metadata[i].variable_type =
                static_cast<data::DataType>(load_unaligned<uint16_t>(begin_ + pointer));
            pointer += sizeof(uint16_t);
        }
        // - 2 ###

        //### 3 - deserialize column_valid_mask
        read_bit_mask(begin_, pointer, column_valid_mask, capacity);
        // - 3 ###

        //### 4 - deserialize data
        const auto data_size_ =
            load_unaligned<uint64_t>(begin_ + pointer);
        pointer += sizeof(uint64_t);
        data.resize(data_size_);

        for (data::DataBatch& data_batch : data) {
            const auto chunks_size =
                load_unaligned<uint64_t>(begin_ + pointer);
            pointer += sizeof(uint64_t);
            data_batch.chunks.resize(chunks_size);

            for (data::ColumnData& column_data : data_batch.chunks) {
                //data
                column_data.data.offset_ =
                    load_unaligned<uint64_t>(begin_ + pointer);
                pointer += sizeof(uint64_t);

                column_data.data.size_ =
                    load_unaligned<uint64_t>(begin_ + pointer);
                pointer += sizeof(uint64_t);

                column_data.data.reference_ =
                    static_cast<PtrReference>(
                            load_unaligned<uint8_t>(begin_ + pointer)
                    );
                pointer += sizeof(uint8_t);

                //offset
                column_data.offsets.offset_ =
                    load_unaligned<uint64_t>(begin_ + pointer);
                pointer += sizeof(uint64_t);

                column_data.offsets.size_ =
                    load_unaligned<uint64_t>(begin_ + pointer);
                pointer += sizeof(uint64_t);

                column_data.offsets.reference_ =
                    static_cast<PtrReference>(
                            load_unaligned<uint8_t>(begin_ + pointer)
                    );
                pointer += sizeof(uint8_t);

                //null_mask
                read_bit_mask(begin_, pointer, column_data.null_mask, capacity);
            }

            //row_mask
            read_bit_mask(begin_, pointer, data_batch.row_mask, capacity);
        }
        // - 4 ###

        // ### 5 -
            valid_rows =
                load_unaligned<uint64_t>(begin_ + pointer);

        // - 5 ###





        storage =
            DataStorage(std::move(main_manager),
                        data_size,
                        PtrReference::Main,
                        atomix_format_main_header_size);



        const std::byte* begin_var = variable_manager.begin();
        size_t pointer_var = 0;

        const auto version_var =
            load_unaligned<std::uint64_t>(begin_var + pointer_var);
        pointer_var += sizeof(std::uint64_t);

        const auto data_size_var =
            load_unaligned<std::uint64_t>(begin_var + pointer_var);
        pointer_var += sizeof(std::uint64_t);

        const auto capacity_var = variable_manager.size();


        variable_storage =
            DataStorage(std::move(variable_manager),
                        data_size_var,
                        PtrReference::Variable,
                        atomix_format_var_header_size);

        return dt;
    }

};



