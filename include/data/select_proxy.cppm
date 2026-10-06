module;
#include <iostream>
#include <ostream>
#include <vector>
#include <memory>

export module atomix.data.select_proxy;
import atomix.data.data_table;

import atomix.data.data_table;
import atomix.bit_mask;
import atomix.data.data_type;
import atomix.data.metadata_data_table;
export namespace atomix::data {
    class SelectProxy {
        std::vector<BitMask> batches_row_mask_;
        BitMask column_mask_;
        DataTable* table_reference_;



        static void dispatch_print( const ColumnData& col_data,
                                    const DataType dt,
                                    const size_t k,
                                    std::byte* ptr,
                                    std::byte* ptr_var){
            switch (dt) {
                case DataType::Int32 :
                    std::cout
                        << *std::start_lifetime_as<int32_dt>(
                            col_data.data.calculate_ptr(k, ptr, sizeof(int32_dt)));

                    break;

                case DataType::Float64 :
                    std::cout
                        << *std::start_lifetime_as<float64_dt>(
                            col_data.data.calculate_ptr(k, ptr, sizeof(float64_dt)));

                    break;

                case DataType::Char :
                    std::cout
                        << *std::start_lifetime_as<char_dt>(
                            col_data.data.calculate_ptr(k, ptr, sizeof(char_dt)));

                    break;


                case DataType::Bool :
                    std::cout
                        << *std::start_lifetime_as<bool_dt>(
                            col_data.data.calculate_ptr(k, ptr, sizeof(bool_dt)));

                    break;

                case DataType::List :
                    throw std::runtime_error("List type is not supported yet");
                    break;

                case DataType::Undefined:
                    throw std::runtime_error("Undefined type is not supported");
                    break;

                default:
                    throw std::runtime_error("Invalid data type");
                    break;

            }


        }


    public:

        explicit SelectProxy():batches_row_mask_({}), column_mask_(BitMask()), table_reference_(nullptr) {}


        explicit SelectProxy(DataTable& table_reference) {
            table_reference_ = &table_reference;
            column_mask_ = detail::unsafe_get_column_valid_mask(table_reference);

            const auto& dt_data =
                detail::unsafe_get_data(table_reference);

            //TODO change to range-based for loop
            batches_row_mask_.reserve(dt_data.size());
            for(size_t i = 0; i < dt_data.size(); ++i) {

                if ( dt_data[i].row_mask.n_of_ones() == 0) {
                    batches_row_mask_.emplace_back();
                }
                else {
                     batches_row_mask_.emplace_back(dt_data[i].row_mask) ;
                }
            }

        }




        void print_result()const {
            auto& data = detail::unsafe_get_data(*table_reference_);
            auto& metadata = detail::unsafe_get_metadata(*table_reference_);
            std::byte* ptr = detail::unsafe_get_storage(*table_reference_).begin();
            std::byte* ptr_var = detail::unsafe_get_variable_storage(*table_reference_).begin();
            // traveling sets of batches
            for(size_t i = 0; i < batches_row_mask_.size(); ++i) {
                const auto& batch_mask = batches_row_mask_[i];
                const auto& src_batch_mask = data[i].row_mask;
                // traveling batches
                if (batch_mask.n_of_ones() != 0 && src_batch_mask.n_of_ones() != 0) {
                    for (size_t j = 0; j < batch_mask.total_bits(); ++j) {
                        if (batch_mask[j] == 1 && src_batch_mask[j] == 1) {
                            // traveling columns
                            for (size_t k = 0; k < metadata.size(); ++k) {
                                std::cout << " [";
                                dispatch_print(data[i].chunks[k], metadata[k].type, j, ptr, ptr_var);
                                std::cout << "]";
                            }
                            std::cout << "\n";
                        }
                    }
                }

            }
        }








    };
}
