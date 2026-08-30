module;
#ifndef NDEBUG

#include <vector>
#include <memory>
#include <cstdint>
#include <cstring>
#include <string>
#include <cassert>
#include <iostream>
#include <cstdlib>
#include <span>


export module atomix.data.data_table:tester;
import :core;
import atomix.data.data_type;
import atomix.mem;
import atomix.config;

export namespace atomix {
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


            DataTable::Column column = DataTable::Column::create_column <DT1, DataType::Undefined>(std::move(name), v.size() * sizeof(type_of_t<DT1>));

            size_t acc = 0;
            for (const auto& buffer : column.buffers) {
                memcpy(buffer.get_begin(),(reinterpret_cast<const uint8_t*> (v.data()) + acc), buffer.get_size());
                acc += buffer.get_size();
            }
            assert(acc == v.size() * sizeof(type_of_t<DT1>));
            std::vector<DataTable::ListMetadata> list_metadata{};
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
        static void artificial_append_variable(DataTable& td, const std::span<T> sp, std::string&& name, const std::vector<uint32_t>& elements_size) {
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

            std::vector<DataTable::ListMetadata> aux_offsets{{std::vector<uint32_t>{0}, 0}};

            size_t acc = 0;
            for (const auto& element_size : elements_size) {
                if (acc + element_size > mem::chunk_size) {
                    aux_offsets.back().last_used_byte = acc;
                    aux_offsets.push_back({std::vector<uint32_t>{0}, 0});
                    acc = 0;
                }
                aux_offsets.back().offsets.push_back(acc);
                acc += element_size;
            }
            aux_offsets.back().last_used_byte = acc;


            const size_t total_size = (aux_offsets.size() - 1) * mem::chunk_size + acc;
            DataTable::Column column = DataTable::Column::create_column <DT1, DT2>(std::move(name), total_size);


            column.n_elements = elements_size.size();

            size_t acc2 = 0;
            for (size_t i = 0; i < aux_offsets.size(); ++i) {
                memcpy(column.buffers[i].get_begin(), reinterpret_cast<uint8_t*>(sp.data()) + acc2, aux_offsets[i].last_used_byte );
                acc2 += aux_offsets[i].last_used_byte;
            }
            assert(acc2 == sp.size_bytes());

            column.list_metadata = std::move(aux_offsets);

            td.columns_.push_back(std::move(column));
        }

    };
}









#endif//NDEBUG