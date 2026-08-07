#ifndef DATATYPE_HH
#define DATATYPE_HH

#include <optional>
#include <string>
#include <stdexcept>
#include <cstdint>
#include <iostream>

namespace atomix {
    enum class DataType : int{
        Int32,
        Float64,
        String,
        Bool,
        Undefined
    };

    //This part is useful for metaprogramming or if we need the raw types for something
    template <DataType DT>
    struct TypeTraits;

    template <>
    struct TypeTraits<DataType::Int32> {
        using type = int32_t;
    };

    template <>
    struct TypeTraits<DataType::Float64> {
        using type = double;
    };

    template <>
    struct TypeTraits<DataType::String> {
        using type = std::string;
    };

    template <>
    struct TypeTraits<DataType::Bool> {
        using type = bool;
    };

    template <DataType DT>
    using type_of_t = TypeTraits<DT>::type;
    //you can get the raw tipe with type_of_t<DT>
}


namespace atomix::data_type_utils {
    inline std::string data_type_to_string(const DataType dt) {

        switch (dt)
        {
            case atomix::DataType::Int32:
                return "Int32";

            case atomix::DataType::Float64:
                return "Float64";

            case atomix::DataType::Bool:
                return "Bool";

            case atomix::DataType::String:
                return "String";

            case atomix::DataType::Undefined:
                return "Undefined";
        }
        throw std::invalid_argument("Invalid data type");

    }

    constexpr std::optional<size_t> byte_size(const DataType dtype){
        switch (dtype)
        {
            case atomix::DataType::Int32:
                return sizeof(type_of_t<DataType::Int32>);

            case atomix::DataType::Float64:
                return sizeof(type_of_t<DataType::Float64>);

            case atomix::DataType::Bool:
                return sizeof(type_of_t<DataType::Bool>);

            case atomix::DataType::String:
                return std::nullopt; //IMPORTANT: returns the size of the offset (uint32), not the string

            case atomix::DataType::Undefined:
                return std::nullopt;
        }
        std::cerr << "Invalid data type" << "\n";
        std::abort();

    }

    constexpr size_t byte_size_fixed(const DataType dtype){
        switch (dtype)
        {
            case atomix::DataType::Int32:
                return sizeof(type_of_t<DataType::Int32>);

            case atomix::DataType::Float64:
                return sizeof(type_of_t<DataType::Float64>);

            case atomix::DataType::Bool:
                return sizeof(type_of_t<DataType::Bool>);

            default:
                std::cerr << "Data type must be fixed size" << "\n";
                std::abort();
        }
    }


}


#endif //DATATYPE_HH