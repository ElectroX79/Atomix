module;
#include <cassert>
#include <string>
#include <cstdint>
#include <vector>
#include <stdexcept>

export module atomix.data.metadata_data_table;
import atomix.data.data_type;
import atomix.mem.chunk;
import atomix.bit_mask;
import atomix.config;
import atomix.mem.chunk;



export namespace atomix::data {

     struct Position {
         uint64_t batch_index;
         uint64_t row_index;

         Position(const uint64_t batch_index1, const uint64_t row_index1):
         batch_index(batch_index1), row_index(row_index1){}

         [[nodiscard]] constexpr size_t to_abs() const {
             return
                (batch_index * n_rows_batch) + row_index;
         }

         static constexpr  Position abs_to_pos(const uint64_t abs_index) {
             return Position
                {abs_index / n_rows_batch, abs_index % n_rows_batch};
         }


         constexpr Position& operator+=(const uint64_t n){
             row_index += n;
             batch_index += row_index / n_rows_batch;
             row_index %= n_rows_batch;
             return *this;
         }

         constexpr Position operator+(const uint64_t n)const{
             Position pos = *this;
             pos += n;
             return pos;
         }


         constexpr Position& operator-=(const uint64_t n){
             const uint64_t current =
                 (n_rows_batch * batch_index) + row_index;

             assert(current >= n && "Position::minus: current => n");
             
             const uint64_t total = current - n;
             row_index = total % n_rows_batch;
             batch_index = total / n_rows_batch;
             return *this;
         }

         constexpr Position operator-(const uint64_t n)const{
             Position pos = *this;
             pos -= n;
             return pos;
         }




         constexpr std::strong_ordering operator<=>(const Position& other) const  {
             if (const auto cmp = batch_index <=> other.batch_index; cmp != 0)
                 return cmp;

             return row_index <=> other.row_index;
         }

         constexpr std::strong_ordering operator<=>(const uint64_t other) const  {
             if (const auto cmp = batch_index <=> other/n_rows_batch ; cmp != 0)
                 return cmp;

             return row_index <=> (other % n_rows_batch);
         }

     };
    struct Column {
        std::string name;
        //uint64_t n_elements = 0; //eliminate
        DataType type = DataType::Undefined;
        DataType variable_type = DataType::Undefined; //CANNOT BE LIST. Also, if type != DataType::List, then variable_type == DataType::Undefined
        Column() = default;
        Column(const std::string_view name1,
            const DataType type1,
            const DataType variable_type1 = DataType::Undefined):
            name(name1),
            type(type1),
            variable_type(variable_type1){}
    };

    struct ColumnData {
        using offset_t = uint64_t;

        mem::Chunk data;
        mem::Chunk offsets;
        BitMask null_mask = BitMask(n_rows_batch, false);


        ColumnData() = default;
        ColumnData(const ColumnData&) = delete;
        ColumnData& operator=(const ColumnData&) = delete;
        ColumnData(ColumnData&&) = default;
        ColumnData& operator=(ColumnData&&) = default;
        ~ColumnData() = default;


        [[nodiscard]]ColumnData unsafe_clone()const {
            ColumnData res;
            res.data = data.unsafe_clone();
            res.offsets = offsets.unsafe_clone();
            res.null_mask = null_mask;
            return res;
        }

        //TODO: study if it needed to convert null_mask from a resident memory-dependent container to a a non-dependent
        //Explanation: rn null_mask consumes a nontrivial part of the resident memory
        //in relation of the total elements, which it may make scalability more difficult
    };

    struct DataBatch {
        // uint64_t n_rows = 0; By default atomix::n_row_batch
        std::vector<ColumnData> chunks = {};
        BitMask row_mask = BitMask(n_rows_batch, false);
        // BitMask null_mask; you are not gonna need it




        DataBatch() = default;

        DataBatch(const DataBatch&) = delete;
        DataBatch& operator=(const DataBatch&) = delete;

        DataBatch(DataBatch&&) noexcept = default;
        DataBatch& operator=(DataBatch&&) noexcept = default;

        ~DataBatch() = default;

        [[nodiscard]]DataBatch unsafe_clone() const{
            DataBatch res;
            for (auto& chunk : chunks) {
                res.chunks.emplace_back(chunk.unsafe_clone());
            }
            res.row_mask = row_mask;
            return res;
        }



    };
}


