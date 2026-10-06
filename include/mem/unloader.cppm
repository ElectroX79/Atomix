module;
#include <cstdint>
#include <cstring>
#include <functional>
#include <cstddef>
#include <string>
#include <vector>

export module atomix.mem.unloader;
import atomix.data.data_type;
import atomix.mem;
import atomix.config;
import atomix.mem.chunk;
import atomix.load_unaligned;
import atomix.mem.file_manager;
import atomix.mem.data_storage;
import atomix.bit_mask;
import atomix.data.metadata_data_table;

namespace atomix::mem {

    size_t write_string(std::byte* begin, const std::string& str, size_t pointer) {
        store_unaligned<uint64_t>(
            begin + pointer,
            static_cast<uint64_t>(str.size())
        );
        pointer += sizeof(uint64_t);

        std::memcpy(
            begin + pointer,
            str.data(),
            str.size()
        );

        return pointer + str.size();
    }

    size_t write_bit_mask(std::byte* begin, const BitMask& bit_mask, size_t pointer) {
        const auto& raw_mask = bit_mask.unsafe_get_mask();
        const uint64_t bit_mask_size = static_cast<uint64_t>(raw_mask.size());

        store_unaligned<uint64_t>(begin + pointer, bit_mask_size);
        pointer += sizeof(uint64_t);
        if (bit_mask_size > 0) {
            std::memcpy(
                begin + pointer,
                raw_mask.data(),
                bit_mask_size * sizeof(mask_t)
            );
        }
        pointer += bit_mask_size * sizeof(mask_t);

        store_unaligned<uint64_t>(begin + pointer, bit_mask.unsafe_get_n_ones());
        pointer += sizeof(uint64_t);

        store_unaligned<uint64_t>(begin + pointer, bit_mask.unsafe_get_abs_size());
        pointer += sizeof(uint64_t);

        return pointer;
    }

    size_t write_chunk(std::byte* begin, const mem::Chunk& chunk, size_t pointer) {
        store_unaligned<uint64_t>(begin + pointer, chunk.offset_);
        pointer += sizeof(uint64_t);

        store_unaligned<uint64_t>(begin + pointer, chunk.size_);
        pointer += sizeof(uint64_t);

        store_unaligned<uint8_t>(begin + pointer, static_cast<uint8_t>(chunk.reference_));
        pointer += sizeof(uint8_t);

        return pointer;
    }
}

export namespace atomix::mem::unloader {

        size_t calculate_metadata_size(
            const std::vector<data::Column>& metadata,
            BitMask& column_valid_mask,
            const std::vector<data::DataBatch>& data
        ) {
            // ### 2 - Columns (8 + k + 2 + 2 = 12 + k)
            size_t total = sizeof(uint64_t); // n_columns
            for (const auto& col : metadata) {
                total += sizeof(uint64_t) + col.name.size(); // name size + name chars
                total += sizeof(uint16_t);                   // type (2 bytes)
                total += sizeof(uint16_t);                   // variable_type (2 bytes)
            }

            // ### 3 - column_valid_mask (24 + k bytes)
            total += sizeof(uint64_t);
            total += column_valid_mask.unsafe_get_mask().size() * sizeof(mask_t);
            total += 2 * sizeof(uint64_t);

            // ### 4 - Data Batches
            total += sizeof(uint64_t); // n_batches
            for (const auto& batch : data) {
                total += sizeof(uint64_t); // chunks_n
                for (const auto& col_data : batch.chunks) {
                    // data Chunk (17 bytes)
                    total += sizeof(uint64_t) * 2 + sizeof(uint8_t);
                    // offsets Chunk (17 bytes)
                    total += sizeof(uint64_t) * 2 + sizeof(uint8_t);
                    // null_mask (24 + k bytes)
                    total += sizeof(uint64_t);
                    total += col_data.null_mask.unsafe_get_mask().size() * sizeof(mask_t);
                    total += 2 * sizeof(uint64_t);
                }
                // row_mask (24 + k bytes)
                total += sizeof(uint64_t);
                total += batch.row_mask.unsafe_get_mask().size() * sizeof(mask_t);
                total += 2 * sizeof(uint64_t);
            }

            // ### 5 - valid_rows (8 bytes)
            total += sizeof(uint64_t);

            return total;
        }


    void save_file(std::vector<data::Column>& metadata,
                    BitMask& column_valid_mask,
                    std::vector<data::DataBatch>& data,
                    size_t& valid_rows,
                    mem::DataStorage& storage,
                    mem::DataStorage& variable_storage
    ) {
        const uint64_t data_size = static_cast<uint64_t>(storage.data_size());
        const uint64_t metadata_offset = data_size;
        const size_t metadata_size = calculate_metadata_size(metadata, column_valid_mask, data);

        if (storage.capacity() - storage.data_size() < metadata_size) {
            storage.reserve(storage.data_size() + metadata_size);
        }

        std::byte* begin = storage.begin();

        // 1. Header
        store_unaligned<uint64_t>(begin, atomix_format_version);
        store_unaligned<uint64_t>(begin + 8, data_size);
        store_unaligned<uint64_t>(begin + 16, metadata_offset);

        // Cleaning padding
        if constexpr (atomix_format_main_header_size > 24) {
            std::memset(begin + 24, 0, atomix_format_main_header_size - 24);
        }

        // 2. Metadata
        size_t pointer = metadata_offset;

        store_unaligned<uint64_t>(begin + pointer, static_cast<uint64_t>(metadata.size()));
        pointer += sizeof(uint64_t);

        for (const auto& col : metadata) {
            pointer = write_string(begin, col.name, pointer);

            store_unaligned<uint16_t>(begin + pointer, static_cast<uint16_t>(col.type));
            pointer += sizeof(uint16_t);

            store_unaligned<uint16_t>(begin + pointer, static_cast<uint16_t>(col.variable_type));
            pointer += sizeof(uint16_t);
        }

        // 3. column_valid_mask
        pointer = write_bit_mask(begin, column_valid_mask, pointer);

        // 4. Data Batches
        store_unaligned<uint64_t>(begin + pointer, static_cast<uint64_t>(data.size()));
        pointer += sizeof(uint64_t);

        for (const auto& batch : data) {
            store_unaligned<uint64_t>(begin + pointer, static_cast<uint64_t>(batch.chunks.size()));
            pointer += sizeof(uint64_t);

            for (const auto& col_data : batch.chunks) {
                pointer = write_chunk(begin, col_data.data, pointer);
                pointer = write_chunk(begin, col_data.offsets, pointer);
                pointer = write_bit_mask(begin, col_data.null_mask, pointer);
            }

            pointer = write_bit_mask(begin, batch.row_mask, pointer);
        }

        // 5. valid_rows
        store_unaligned<uint64_t>(begin + pointer, static_cast<uint64_t>(valid_rows));

            
        // 6. UPdate header
        std::byte* begin_var = variable_storage.begin();
        if (begin_var != nullptr) {
            size_t p_var = 0;
            store_unaligned<uint64_t>(begin_var + p_var, atomix_format_version);
            p_var += sizeof(uint64_t);

            store_unaligned<uint64_t>(begin_var + p_var, static_cast<uint64_t>(variable_storage.data_size()));
            p_var += sizeof(uint64_t);
        }
    }

}