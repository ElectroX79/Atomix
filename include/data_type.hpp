#ifndef DATATYPE_HH
#define DATATYPE_HH

#include <optional>
#include <string>
#include <stdexcept>
#include <cstdint>

namespace Atomix {
    enum class DataType : int{
        Int32,
        Float64,
        String,
        Bool,
        Undefined
    };
}


namespace Atomix::DataTypeUtils {
    inline std::string data_type_to_string(const DataType& dt) {

        switch (dt)
        {
            case DataType::Int32:
                return "Int32";

            case DataType::Float64:
                return "Float64";

            case DataType::Bool:
                return "Bool";

            case DataType::String:
                return "String";

            case DataType::Undefined:
                return "Undefined";
        }
        throw std::invalid_argument("Invalid data type");

    }

    constexpr std::optional<size_t> byte_size(DataType dtype){
        switch (dtype)
        {
            case DataType::Int32:
                return sizeof(int32_t);

            case DataType::Float64:
                return sizeof(double);

            case DataType::Bool:
                return sizeof(bool);

            case DataType::String:
                return std::nullopt; //IMPORTANT: returns the size of the offset (uint32), not the string

            case DataType::Undefined:
                return std::nullopt;
        }
        throw std::invalid_argument("Invalid data type");

    }
}


#endif //DATATYPE_HH