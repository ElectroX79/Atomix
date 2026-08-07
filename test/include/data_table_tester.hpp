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
        template <typename T>


        /**
         * @brief Appends a test column to a TabularData object from a vector of fixed-size values.
         * @tparam T Type of the values stored in the input vector.
         * @param td Target TabularData object to modify.
         * @param v Values to copy into the new column buffer.
         * @param data_t Fixed-size data type describing the column values.
         * @param name Name of the column to append.
         */

        static void artificial_append(DataTable& td, const std::vector<T>& v ,const DataType data_t, std::string name){
            if (v.empty()) {
                return;
            }
            const size_t byte_size = data_type_utils::byte_size_fixed(data_t);
            
            size_t chunk_size;
            std::vector<std::shared_ptr<mem::Buffer>> buffers = mem::mem_route::allocate(v.size() * byte_size, chunk_size, 64);


            size_t acc = 0;
            for (const auto& buffer : buffers) {
                memcpy(buffer->get_begin(),(reinterpret_cast<const uint8_t*> (v.data()) + acc), buffer->size());
                acc += buffer->size();
            }

            assert(acc == v.size() * byte_size);

            const DataTable::Column column(std::move(name), std::move (buffers), v.size(), chunk_size, data_t);
            td.columns_.push_back(column);
        }

    };
}










#endif //TABULAR_DATA_TESTER_HPP

#endif//NDEBUG