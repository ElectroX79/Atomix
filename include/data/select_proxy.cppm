module;
#include <cassert>
#include <iostream>
#include <ostream>
#include <vector>
#include <memory>
#include <valarray>

export module atomix.data.select_proxy;
import atomix.data.data_table;

import atomix.data.data_table;
import atomix.bit_mask;
import atomix.data.data_type;
import atomix.data.metadata_data_table;
import atomix.bounds;
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


        template<DataType DT>
        auto sum_col(const ColumnData& col_data,
                    const BitMask& mask, std::byte* begin,
                    const DataType col_type )
                    const -> type_of_t<DT> {

            using DT_T = type_of_t<DT>;

            static_assert(data_type_utils::is_numeric(DT));
            assert(data_type_utils::is_numeric(col_type));

            const size_t type_size = data_type_utils::byte_size_fixed(col_type);

            assert(col_data.data.size_ % type_size == 0);


            const size_t norm_size =
                col_data.data.size_ / type_size ;


            assert(mask.total_bits() == norm_size);

            std::byte* raw_ptr =
                col_data.data.calculate_ptr(0, begin, type_size );



            DT_T sum = 0;
            static_assert(mask_t_bits % 4 == 0);

            if(col_type == DataType::Float64) {

                using COL_T = type_of_t<DataType::Float64>;
                const COL_T* __restrict__ ptr =
                    std::start_lifetime_as_array<COL_T>(raw_ptr, norm_size);

                COL_T sum0 = 0;
                COL_T sum1 = 0;
                COL_T sum2 = 0;
                COL_T sum3 = 0;
                COL_T sum_r = 0;


                const size_t limit = norm_size - norm_size % mask_t_bits;

                for (size_t i = 0; i < limit ; i+=mask_t_bits) {
                    const mask_t word =
                        mask.get_word(i/mask_t_bits);

                    if (word == ~mask_t(0)) {
                    //if constexpr (false){
                        #pragma GCC ivdep
                        #pragma GCC unroll 4
                        for (size_t j = i; j < i + mask_t_bits; j+=4) {
                            sum0 += ptr[j+0];
                            sum1 += ptr[j+1];
                            sum2 += ptr[j+2];
                            sum3 += ptr[j+3];
                        }
                    }

                    else if (word == mask_t(0)) {
                    //if constexpr (false){
                        continue;
                    }

                    else {
                        const mask_t word2 = word;
                        for (size_t j = i; j < i + mask_t_bits; j += 4) {
                            const mask_t word2_offset = word2 >> (j - i);
                            sum0 +=
                                word2_offset & (static_cast<mask_t>(1) << 0)
                                ? ptr[j] : static_cast<COL_T>(0);
                            sum1 +=
                                word2_offset  & (static_cast<mask_t>(1) << 1)
                                ? ptr[j + 1] : static_cast<COL_T>(0);
                            sum2 +=
                                word2_offset  & (static_cast<mask_t>(1) << 2)
                                ? ptr[j + 2] : static_cast<COL_T>(0);
                            sum3 +=
                                word2_offset  & (static_cast<mask_t>(1) << 3)
                                ? ptr[j + 3] : static_cast<COL_T>(0);
                        }
                    }

                }

                for (size_t i = limit; i < norm_size; ++i) {
                    sum_r += mask[i] ? ptr[i] : 0;
                }

                sum = static_cast<DT_T>(sum_r + sum0 + sum1 + sum2 + sum3);
            }
            else if (col_type == DataType::Int32) {


                using COL_T = type_of_t<DataType::Int32>;
                const COL_T* __restrict__ ptr  =
                    std::start_lifetime_as_array<COL_T>(raw_ptr, norm_size);

                COL_T sum0 = 0;
                COL_T sum1 = 0;
                COL_T sum2 = 0;
                COL_T sum3 = 0;
                COL_T sum_r = 0;


                const size_t limit = norm_size - norm_size % mask_t_bits;

                for (size_t i = 0; i < limit ; i+=mask_t_bits) {
                    const mask_t word =
                        mask.get_word(i/mask_t_bits);

                    if (word == ~mask_t(0)) {
                    //if constexpr (false){
                        for (size_t j = i; j < i + mask_t_bits; j+=4) {
                            sum0 += ptr[j+0];
                            sum1 += ptr[j+1];
                            sum2 += ptr[j+2];
                            sum3 += ptr[j+3];
                        }
                    }

                    else if (word == mask_t(0)) {
                    //else if constexpr (false){
                        continue;
                    }

                    else {
                        const mask_t word2 = word;
                        for (size_t j = i; j < i + mask_t_bits; j += 4) {
                            const mask_t word2_offset = word2 >> (j - i);
                            sum0 +=
                                word2_offset & (static_cast<mask_t>(1) << 0)
                                ? ptr[j] : static_cast<COL_T>(0);
                            sum1 +=
                                word2_offset  & (static_cast<mask_t>(1) << 1)
                                ? ptr[j + 1] : static_cast<COL_T>(0);
                            sum2 +=
                                word2_offset  & (static_cast<mask_t>(1) << 2)
                                ? ptr[j + 2] : static_cast<COL_T>(0);
                            sum3 +=
                                word2_offset  & (static_cast<mask_t>(1) << 3)
                                ? ptr[j + 3] : static_cast<COL_T>(0);
                        }
                    }

                }

                for (size_t i = limit; i < norm_size; ++i) {
                    sum_r += mask[i] ? ptr[i] : 0;
                }

                sum = static_cast<DT_T>(sum_r + sum0 + sum1 + sum2 + sum3);
            }


            return sum;

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
            if (table_reference_ == nullptr) {
                throw std::runtime_error("SelectProxy::col_sum: table_reference_ is nullptr");
            }

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


        template<DataType DT>
        [[nodiscard]]type_of_t<DT> col_sum(size_t col_index1) const {
            if (table_reference_ == nullptr) {
                throw std::runtime_error("SelectProxy::col_sum: table_reference_ is nullptr");
            }

            const auto& data = detail::unsafe_get_data(*table_reference_);
            const auto& metadata = detail::unsafe_get_metadata(*table_reference_);

            bounds::check_index_individual(col_index1, metadata.size());

            const DataType data_t = metadata[col_index1].type;

            if (!data_type_utils::is_numeric(data_t)) {
                throw std::runtime_error(
                    "Invalid data type, that type do not admit math operation");
            }

            if (data.size() != batches_row_mask_.size()) {
                throw std::runtime_error("Invalid data size, reference was changed, this SelectProxy is probably invalid or corrupted");
            }



            type_of_t<DT> sum = 0;

            // traveling sets of batches
            std::byte* begin =
                detail::unsafe_get_storage(*table_reference_).begin();

            for(size_t i = 0; i < batches_row_mask_.size(); ++i) {
                const auto& batch_mask = batches_row_mask_[i];
                const auto& batch_data = data[i];

                if (batch_data.row_mask.n_of_ones() == 0 || batch_mask.n_of_ones() == 0) {
                    continue;
                }
                const auto& col_to_op = batch_data.chunks[col_index1]; //column to operate
                const auto bit_mask = (batch_data.row_mask & batch_mask & col_to_op.validity_mask);

                if (bit_mask.n_of_ones() == 0) {
                    continue;
                }

                sum +=
                    sum_col<DT>(
                        col_to_op,
                        bit_mask,
                        begin,
                        data_t);
            }

            return sum;
        }








    };
}
