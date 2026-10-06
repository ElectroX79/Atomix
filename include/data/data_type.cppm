module;

#include <optional>
#include <string>
#include <stdexcept>
#include <cstdint>
#include <iostream>
#include <cstdlib>
#include <variant>
#include <cstddef>
#include <utility>


export module atomix.data.data_type;

export namespace atomix::data {
    enum class DataType : uint16_t{
        Int32,
        Float64,
        List,
        Bool,
        Char,
        Undefined
    };
    
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
    using type_of_t = typename TypeTraits<DT>::type;

    using int32_dt   = type_of_t<DataType::Int32>;
    using float64_dt = type_of_t<DataType::Float64>;
    using bool_dt    = type_of_t<DataType::Bool>;
    using char_dt    = type_of_t<DataType::Char>;


}

export namespace atomix::data {
    struct CompDataType {
        DataType type;
        DataType list_type;
    };

    //TODO: improve this, later
    template<typename T>
    concept TDataTypeCompatible = std::is_same_v<type_of_t<DataType::Int32>, T>
                                || std::is_same_v<type_of_t<DataType::Float64>, T>
                                || std::is_same_v<type_of_t<DataType::Bool>, T>
                                || std::is_same_v<type_of_t<DataType::Char>, T>
                                || std::is_same_v<std::span<type_of_t<DataType::Int32>>, T>
                                || std::is_same_v<std::span<type_of_t<DataType::Float64>>, T>
                                || std::is_same_v<std::span<type_of_t<DataType::Bool>>, T>
                                || std::is_same_v<std::span<type_of_t<DataType::Char>>, T>;

    template<typename T>
    concept TDataTypeCompatibleCol =
        std::is_same_v<std::span<type_of_t<DataType::Int32>>, std::remove_cvref_t<T>>
     || std::is_same_v<std::span<type_of_t<DataType::Float64>>, std::remove_cvref_t<T>>
     || std::is_same_v<std::span<type_of_t<DataType::Bool>>, std::remove_cvref_t<T>>
     || std::is_same_v<std::span<type_of_t<DataType::Char>>, std::remove_cvref_t<T>>
     || std::is_same_v<std::span<std::span<type_of_t<DataType::Int32>>>, std::remove_cvref_t<T>>
     || std::is_same_v<std::span<std::span<type_of_t<DataType::Float64>>>, std::remove_cvref_t<T>>
     || std::is_same_v<std::span<std::span<type_of_t<DataType::Bool>>>, std::remove_cvref_t<T>>
     || std::is_same_v<std::span<std::span<type_of_t<DataType::Char>>>, std::remove_cvref_t<T>>;


    template<TDataTypeCompatible T>
    constexpr CompDataType type_to_datatype() { //this function its not safe at all, use with precaution
        if constexpr (std::is_same_v<T, type_of_t<DataType::Int32>>) {
            return {DataType::Int32, DataType::Undefined};
        }
        if constexpr (std::is_same_v<T, type_of_t<DataType::Float64>>) {
            return {DataType::Float64, DataType::Undefined};
        }
        if constexpr (std::is_same_v<T, type_of_t<DataType::Bool>>) {
            return {DataType::Bool, DataType::Undefined};
        }
        if constexpr (std::is_same_v<T, type_of_t<DataType::Char>>) {
            return {DataType::Char, DataType::Undefined};
        }
        if constexpr (std::is_same_v<T, std::span<type_of_t<DataType::Int32>>>) {
            return {DataType::List, DataType::Int32};
        }
        if constexpr (std::is_same_v<T, std::span<type_of_t<DataType::Float64>>>) {
            return {DataType::List, DataType::Float64};
        }
        if constexpr (std::is_same_v<T, std::span<type_of_t<DataType::Bool>>>) {
            return {DataType::List, DataType::Bool};
        }
        if constexpr (std::is_same_v<T, std::span<type_of_t<DataType::Char>>>) {
            return {DataType::List, DataType::Char};
        }
        std::unreachable();
    }


    template<typename T>
   constexpr CompDataType type_to_datatype_col() { //this function its not safe at all, use with precaution
        if constexpr (std::is_same_v<T, std::span<type_of_t<DataType::Int32>>>) {
            return {DataType::Int32, DataType::Undefined};
        }
        if constexpr (std::is_same_v<T, std::span<type_of_t<DataType::Float64>>>) {
            return {DataType::Float64, DataType::Undefined};
        }
        if constexpr (std::is_same_v<T, std::span<type_of_t<DataType::Bool>>>) {
            return {DataType::Bool, DataType::Undefined};
        }
        if constexpr (std::is_same_v<T, std::span<type_of_t<DataType::Char>>>) {
            return {DataType::Char, DataType::Undefined};
        }
        if constexpr (std::is_same_v<T, std::span<std::span<type_of_t<DataType::Int32>>>>) {
            return {DataType::List, DataType::Int32};
        }
        if constexpr (std::is_same_v<T, std::span<std::span<type_of_t<DataType::Float64>>>>) {
            return {DataType::List, DataType::Float64};
        }
        if constexpr (std::is_same_v<T, std::span<std::span<type_of_t<DataType::Bool>>>>) {
            return {DataType::List, DataType::Bool};
        }
        if constexpr (std::is_same_v<T, std::span<std::span<type_of_t<DataType::Char>>>>) {
            return {DataType::List, DataType::Char};
        }
        return{DataType::Undefined, DataType::Undefined};
    }

    template<DataType DT>
    concept DT_is_fixed_t = DT != DataType::List;

    using TGroup =
       std::variant<
           type_of_t<DataType::Int32>,
           type_of_t<DataType::Float64>,
           type_of_t<DataType::Bool>,
           type_of_t<DataType::Char>,

            //DataType == List
           std::span<type_of_t<DataType::Int32>>,
           std::span<type_of_t<DataType::Float64>>,
           std::span<type_of_t<DataType::Bool>>,
           std::span<type_of_t<DataType::Char>>
       >;

    using TGroupColumn =
        std::variant<
            std::span<type_of_t<DataType::Int32>>,
            std::span<type_of_t<DataType::Float64>>,
            std::span<type_of_t<DataType::Bool>>,
            std::span<type_of_t<DataType::Char>>,

            //DataType == List
            std::span<std::span<type_of_t<DataType::Int32>>>,
            std::span<std::span<type_of_t<DataType::Float64>>>,
            std::span<std::span<type_of_t<DataType::Bool>>>,
            std::span<std::span<type_of_t<DataType::Char>>>
        >;
    //dangerous, semi-deprecated, use std::visit instead if it's possible
    CompDataType get_data_type(const TGroup& value) {
        switch (value.index()) {
            case 0: return {DataType::Int32, DataType::Undefined};
            case 1: return {DataType::Float64, DataType::Undefined};
            case 2: return {DataType::Bool, DataType::Undefined};
            case 3: return {DataType::Char, DataType::Undefined};

            case 4: return {DataType::List, DataType::Int32};
            case 5: return {DataType::List, DataType::Float64};
            case 6: return {DataType::List, DataType::Bool};
            case 7: return {DataType::List, DataType::Char};

            default: return {DataType::Undefined, DataType::Undefined};
        }


    }

    //dangerous, semi-deprecated, use std::visit instead if it's possible
    CompDataType get_data_type(const TGroupColumn& value) {
        switch (value.index()) {
            case 0: return {DataType::Int32, DataType::Undefined};
            case 1: return {DataType::Float64, DataType::Undefined};
            case 2: return {DataType::Bool, DataType::Undefined};
            case 3: return {DataType::Char, DataType::Undefined};

            case 4: return {DataType::List, DataType::Int32};
            case 5: return {DataType::List, DataType::Float64};
            case 6: return {DataType::List, DataType::Bool};
            case 7: return {DataType::List, DataType::Char};

            default: return {DataType::Undefined, DataType::Undefined};
    }


    }

}


export namespace atomix::data::data_type_utils {
    inline std::string data_type_to_string(const DataType dt) {

        switch (dt)
        {
            case DataType::Int32:
                return "Int32";

            case DataType::Float64:
                return "Float64";

            case DataType::Bool:
                return "Bool";

            case DataType::List:
                return "List";

            case DataType::Char:
                return "Char";

            case DataType::Undefined:
                return "Undefined";
        }
        throw std::invalid_argument("Invalid data type");

    }

    constexpr std::optional<size_t> byte_size(const DataType dtype){
        switch (dtype)
        {
            case DataType::Int32:
                return sizeof(type_of_t<DataType::Int32>);

            case DataType::Float64:
                return sizeof(type_of_t<DataType::Float64>);

            case DataType::Bool:
                return sizeof(type_of_t<DataType::Bool>);

            case DataType::Char:
                return sizeof(type_of_t<DataType::Char>);

            case DataType::List:
                return std::nullopt;

            case DataType::Undefined:
                return std::nullopt;
        }
        std::cerr << "Invalid data type" << "\n";
        std::abort();

    }
    // TODO: use byte_size inside of byte_size_fixed
    constexpr size_t byte_size_fixed(const DataType dtype){
        switch (dtype)
        {
            case DataType::Int32:
                return sizeof(type_of_t<DataType::Int32>);

            case DataType::Float64:
                return sizeof(type_of_t<DataType::Float64>);

            case DataType::Bool:
                return sizeof(type_of_t<DataType::Bool>);

            case DataType::Char:
                return sizeof(type_of_t<DataType::Char>);

            default:
                std::cerr << "Data type must be fixed size" << "\n";
                std::abort();
        }
    }


}

