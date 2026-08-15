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
        * @brief Appends a fixed-size test column to a DataTable.
        *
        * @tparam DT1 Column data type. Must not be DataType::List.
        * @tparam T Input value type. Must match type_of_t<DT1>.
        * @param td Target table to modify.
        * @param v Values copied into the new column.
        * @param name Name of the appended column.
        *
        * @note Does nothing if @p v is empty. Aborts on contract violations.
        */
        template <DataType DT1, typename T>
        static void artificial_append_fixed(DataTable& td, const std::vector<T>& v, std::string name){


            if constexpr(DT1 == DataType::List) {
                std::cerr << "Contract violation: artificial_append_fixed does not accept variable type columns )" << "\n";
                std::abort();
            }

            if constexpr(!(std::is_same_v<type_of_t<DT1>, T>)){
                std::cerr << "Contract violation: data type mismatch (artificial_append_fixed())" << "\n";
                std::abort();
            }

            if (v.empty()) {
                return;
            }


            const size_t byte_size = data_type_utils::byte_size_fixed(DT1);
            size_t chunk_size;
            std::vector<std::shared_ptr<mem::Buffer>> buffers = mem::mem_route::allocate(v.size() * byte_size, chunk_size, 64);

            size_t acc = 0;
            for (const auto& buffer : buffers) {
                memcpy(buffer->get_begin(),(reinterpret_cast<const uint8_t*> (v.data()) + acc), buffer->get_size());
                acc += buffer->get_size();
            }
            assert(acc == v.size() * byte_size);
            std::vector<DataTable::ListMetadata> list_metadata{};
            const DataTable::Column column(std::move(name), std::move (buffers), std::move(list_metadata), v.size(), chunk_size, DT1, DataType::Undefined);
            td.columns_.push_back(column);

        }


            /**
         * @brief Appends a variable-size list test column to a DataTable.
         *
         * @tparam DT1 Column data type. Must be DataType::List.
         * @tparam DT2 List element data type.
         * @tparam T Input value type. Must match type_of_t<DT2>.
         * @param td Target table to modify.
         * @param sp Variable-size  span.
         * @param name Name of the appended column.
         * @param elements_size Each element byte-size  element_size
         *
         * @note Does nothing if @p sp is empty. Aborts on contract violations.
         */
        template <DataType DT1, DataType DT2, typename T>
        static void artificial_append_variable(DataTable& td, const std::span<T> sp, std::string name, const std::vector<uint32_t> elements_size) {
            if (sp.empty()) {
                return;
            }

            if constexpr(DT1 != DataType::List){
                std::cerr << "Contract violation: artificial_append_variable does not accept fixed type columns )" << "\n";
                std::abort();
            }

            if constexpr(!(std::is_same_v<type_of_t<DT2>, T>)) { //notice that there is comparing DT2, not DT1
                std::cerr << "Contract violation: data type mismatch (artificial_append_variable())" << "\n";
                std::abort();
            }

            constexpr size_t kb = 1024;
            constexpr uint64_t chunk_size_aux = 64 * kb; //hardcoded, see mem_route as reference

            std::vector<DataTable::ListMetadata> aux_offsets{{std::vector<uint32_t>{0}, 0}};

            size_t acc = 0;
            for (const auto& element_size : elements_size) {
                if (acc + element_size > chunk_size_aux) {
                    aux_offsets.back().last_used_byte = acc;
                    aux_offsets.push_back({std::vector<uint32_t>{0}, 0});
                    acc = 0;
                }
                aux_offsets.back().offsets.push_back(acc);
                acc += element_size;
            }
            aux_offsets.back().last_used_byte = acc;




            size_t chunk_size;
            const size_t total_size = (aux_offsets.size() - 1) * chunk_size_aux + acc;

            std::vector<std::shared_ptr<mem::Buffer>> buffers = mem::mem_route::allocate(total_size, chunk_size, 64);
            const DataTable::Column column( std::move(name),
                                            std::move(buffers),
                                            std::move(aux_offsets),
                                            elements_size.size(), chunk_size,
                                            DT1,
                                            DT2);
            td.columns_.push_back(std::move(column));
        }

    };
}










#endif //TABULAR_DATA_TESTER_HPP

#endif//NDEBUG