#ifndef DATA_TABLE_HPP
#define DATA_TABLE_HPP

#include <vector>
#include <stdexcept>
#include <string_view>
#include <memory>


#include "data_type.hpp"
#include "buffer.hpp"
#include "security_check.hpp"

namespace Atomix {
    class DataTable{

        struct Column {
            std::string name ;
            std::shared_ptr<atomix::Buffer> buffer = nullptr;
            DataType type = DataType::Undefined;

            Column(const Column& other) = default;
            Column(Column&& other) = default;
            Column& operator=(const Column& other) = default;
            Column& operator=(Column&& other) = default;
        };


        std::vector<Column> columns_;




    public:

        DataTable() = default;

        DataTable(const DataTable& other) = default;

        DataTable& operator=(const DataTable& other)noexcept {
            if (this != &other){
                columns_ = other.columns_;
            }
            return *this;
        }

        DataTable(DataTable&& other) = default;

        DataTable& operator=(DataTable&& other)noexcept {
            if (this != &other){
                columns_ = std::move(other.columns_);
            }
            return *this;
        }



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
            return columns_[index].buffer->size();
        }



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