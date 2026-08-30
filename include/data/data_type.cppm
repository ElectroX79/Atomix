module;

#include <optional>
#include <string>
#include <stdexcept>
#include <cstdint>
#include <iostream>
#include <cstdlib>


export module atomix.data.data_type;

export namespace atomix {
    enum class DataType : uint8_t{
        Int32,
        Float64,
        List,
        Bool,
        Char,
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
    struct TypeTraits<DataType::Bool> {
        using type = bool;
    };

    template <>
    struct TypeTraits<DataType::Char> {
        using type = char;
    };

    template <DataType DT>
    using type_of_t = TypeTraits<DT>::type;
    //you can get the raw tipe with type_of_t<DT>
}


export namespace atomix::data_type_utils {
    inline std::string data_type_to_string(const DataType dt) {

        switch (dt)
        {
            case atomix::DataType::Int32:
                return "Int32";

            case atomix::DataType::Float64:
                return "Float64";

            case atomix::DataType::Bool:
                return "Bool";

            case atomix::DataType::List:
                return "List";

            case atomix::DataType::Char:
                return "Char";

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

            case atomix::DataType::Char:
                return sizeof(type_of_t<DataType::Char>);

            case atomix::DataType::List:
                return std::nullopt;

            case atomix::DataType::Undefined:
                return std::nullopt;
        }
        std::cerr << "Invalid data type" << "\n";
        std::abort();

    }
    // TODO: use byte_size inside of byte_size_fixed
    constexpr size_t byte_size_fixed(const DataType dtype){
        switch (dtype)
        {
            case atomix::DataType::Int32:
                return sizeof(type_of_t<DataType::Int32>);

            case atomix::DataType::Float64:
                return sizeof(type_of_t<DataType::Float64>);

            case atomix::DataType::Bool:
                return sizeof(type_of_t<DataType::Bool>);

            case atomix::DataType::Char:
                return sizeof(type_of_t<DataType::Char>);

            default:
                std::cerr << "Data type must be fixed size" << "\n";
                std::abort();
        }
    }


}

