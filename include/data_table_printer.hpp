#ifndef DATA_TABLE_PRINTER
#define DATA_TABLE_PRINTER

#include <iostream>
#include "data_table.hpp"
#include "data_type.hpp"


namespace atomix{
    struct DataTablePrinter{
        static void metadata(const DataTable& dt) {
            for (size_t i = 0; i < dt.columns_.size(); ++i) {
                std::cout << dt.column_name(i) << ": ";
                std::cout << "n_elements " << dt.column_size(i) ;
                std::cout << ", datatype " <<data_type_utils::data_type_to_string(dt.column_datatype(i));
                if (dt.column_datatype(i) == DataType::List) {
                    std::cout << ", list subtype " << data_type_utils::data_type_to_string(dt.columns_[i].variable_type);
                }
                std::cout << std::endl;
            }
        }
        static void data(const DataTable& dt) {
            for (size_t i = 0; i < dt.columns_.size(); ++i) {
                std::cout << dt.column_name(i) << ": ";
                const auto opt = data_type_utils::byte_size(dt.column_datatype(i));
                if (!opt.has_value()) {
                    std::cerr << "No suitable data type (it needs to be a fixed size data_type)" << std::endl;
                    abort();
                }
                for (const auto& buffer : dt.columns_[i].buffers ) {
                    for (size_t k = 0; k < buffer->get_size(); k+=opt.value()) {

                        switch (dt.column_datatype(i)) {
                            case DataType::Bool:
                                std::cout << (buffer->get_begin()[k] == 0 ? "false" : "true") << " ";
                                break;

                            case DataType::Int32:
                                std::cout << *(reinterpret_cast<int32_t*> (buffer->get_begin() + k)) << " ";
                                break;

                            case DataType::Float64:
                                std::cout << *(reinterpret_cast<double*> (buffer->get_begin() + k)) << " ";
                                break;

                            default:
                                std::cerr << "List (unsupported)" << std::endl;
                        }
                    }
                }
                std::cout << std::endl;
            }
        }
    };


}

#endif
