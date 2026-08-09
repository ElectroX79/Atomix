#ifndef NDEBUG


#ifndef TABULAR_DATA_TESTER_HPP
#define TABULAR_DATA_TESTER_HPP

#include <vector>
#include <memory>
#include <cstdint>
#include <cstring>
#include <string>
#include <cassert>


#include "data_table.hpp"
#include "data_type.hpp"
#include "mem/mem_route.hpp"
#include "mem/buffer.hpp"


namespace atomix {
    class DataTableTester{
    public:



        /**
         * @brief Appends a test column to a TabularData object from a vector of fixed-size values.
         * @tparam T Type of the values stored in the input vector.
         * @param td Target TabularData object to modify.
         * @param v Values to copy into the new column buffer.
         * @param data_t Fixed-size data type describing the column values.
         * @param name Name of the column to append.
         */
        template <typename T>
        static void artificial_append(DataTable& td, const std::vector<T>& v, const DataType data_t, std::string name){
            if (v.empty()) {
                return;
            }

            

            if (data_t != DataType::List) {
                const size_t byte_size = data_type_utils::byte_size_fixed(data_t);
                size_t chunk_size;
                std::vector<std::shared_ptr<mem::Buffer>> buffers = mem::mem_route::allocate(v.size() * byte_size, chunk_size, 64);

                size_t acc = 0;
                for (const auto& buffer : buffers) {
                    memcpy(buffer->get_begin(),(reinterpret_cast<const uint8_t*> (v.data()) + acc), buffer->get_size());
                    acc += buffer->get_size();
                }
                assert(acc == v.size() * byte_size);
                std::vector<DataTable::ListMetadata> list_metadata{};
                const DataTable::Column column(std::move(name), std::move (buffers), std::move(list_metadata), v.size(), chunk_size, data_t);
                td.columns_.push_back(column);
            }


            else if constexpr (std::is_same_v<T, std::string>) {
                size_t chunk_size;
                size_t size = 0;
                for (const auto& str : v) {
                    size += str.size();
                }
                std::vector<std::shared_ptr<mem::Buffer>> buffers = mem::mem_route::allocate(size, chunk_size, 64);
                std::vector<DataTable::ListMetadata> list_metadata(buffers.size());

                if (chunk_size == 0) {
                    size_t v_index = 0;
                    size_t buffer_index = 0;
                    size_t acc = 0;
                    DataTable::ListMetadata metadata;
                    metadata.type = DataType::Char;
                    while ( v_index < v.size()){
                        metadata.offsets.push_back(acc);
                        std::memcpy(buffers[buffer_index]->get_begin() + acc, v[v_index].c_str(), v[v_index].size());

                        acc += v[v_index].size();
                        v_index++;
                    }
                    metadata.last_used_byte = acc;
                    list_metadata[buffer_index] = (metadata);

                    const DataTable::Column column(std::move(name), std::move (buffers), std::move(list_metadata), v.size(), chunk_size, data_t);
                    td.columns_.push_back(column);
                    return;
                }


                size_t v_index = 0;
                size_t buffer_index = 0;
                while ( v_index < v.size()) {
                    size_t acc = 0;
                    DataTable::ListMetadata metadata;
                    metadata.type = DataType::Char;
                    while ( v_index < v.size() && acc + v[v_index].size() <= chunk_size){
                        metadata.offsets.push_back(acc);
                        std::memcpy(buffers[buffer_index]->get_begin() + acc, v[v_index].c_str(), v[v_index].size());

                        acc += v[v_index].size();
                        v_index++;
                    }
                    metadata.last_used_byte = acc;
                    list_metadata[buffer_index] = (metadata);
                    buffer_index++;
                }
                const DataTable::Column column(std::move(name), std::move (buffers), std::move(list_metadata), v.size(), chunk_size, data_t);
                td.columns_.push_back(column);
            }
            else {
                throw(std::invalid_argument(" Need to be fixed type or string"));
            }





        }

    };
}










#endif //TABULAR_DATA_TESTER_HPP

#endif//NDEBUG