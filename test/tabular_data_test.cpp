#include <stdexcept>
#include <cstdint>

#include "catch_amalgamated.hpp"
#include "tabular_data.hpp"



template<typename T>
std::vector<uint8_t> to_bytes(T value) {
    std::vector<uint8_t> bytes(sizeof(T));
    std::memcpy(bytes.data(), &value, sizeof(T));
    return bytes;
}

DataTable create_test_instance() {
    std::vector<uint8_t> raw_data;
    
    auto int_bytes = to_bytes<int32_t>(42);
    raw_data.insert(raw_data.end(), int_bytes.begin(), int_bytes.end());

    auto padding_bytes = to_bytes<uint16_t>(0);
    raw_data.insert(raw_data.end(), padding_bytes.begin(), padding_bytes.end());
    
    auto str_offset_bytes = to_bytes<uint64_t>(0);
    raw_data.insert(raw_data.end(), str_offset_bytes.begin(), str_offset_bytes.end());

    std::vector<DataTable::meta_data> metadata = {
        {"id", 0, 0, 1, DataType::Int32},
        {"name", 4, 8, 1, DataType::String}
    };

    std::vector<std::string> string_buffer = {"Hola"};

    DataTable td(raw_data, metadata, string_buffer);
    return td;
}

DataTable create_test_instance2() {
    std::vector<uint8_t> raw_data;


    auto int_bytes = to_bytes<int32_t>(42);
    raw_data.insert(raw_data.end(), int_bytes.begin(), int_bytes.end());

    auto padding_bytes = to_bytes<uint16_t>(0);
    raw_data.insert(raw_data.end(), padding_bytes.begin(), padding_bytes.end());


    auto int_bytes2 = to_bytes<int32_t>(42);
    raw_data.insert(raw_data.end(), int_bytes2.begin(), int_bytes2.end());

    auto padding_bytes2 = to_bytes<uint16_t>(0);
    raw_data.insert(raw_data.end(), padding_bytes2.begin(), padding_bytes2.end());


    auto str_offset_bytes2 = to_bytes<uint64_t>(0);
    raw_data.insert(raw_data.end(), str_offset_bytes2.begin(), str_offset_bytes2.end());


    std::vector<DataTable::meta_data> metadata = {
        {"id", 0, 0, 1, DataType::Int32},
        {"name", 4, 8, 1, DataType::String},
        {"id", 0, 16, 1, DataType::Int32},
        {"name", 4, 24, 1, DataType::String}
    };

    std::vector<std::string> string_buffer = {"Hola", "Hola"};

    DataTable td(raw_data, metadata, string_buffer);
    return td;
}



TEST_CASE("Basic memory integrity test", "[tabular_data]"){
    


    SECTION("START_EMPTINESS_TEST"){
        DataTable td;
        REQUIRE(td.empty());
    }

    SECTION("START_NON_EMPTY"){

        DataTable td;
        REQUIRE(td.empty());
    }



    
}

TEST_CASE("Exception handling test", "[tabular_data]"){
    DataTable td;


    SECTION("OUT_BOUNDS_TEST_ERASE"){
        REQUIRE_THROWS_AS(td.erase(3, 2), std::invalid_argument);
    }

    SECTION("OUT_BOUNDS_TEST_EXTRACT"){
        REQUIRE_THROWS_AS(td.extract(3, 2), std::invalid_argument);
    }


}


TEST_CASE("Appending and erasing", "[tabular_data]"){
    DataTable td = create_test_instance();
    DataTable td2 = create_test_instance2();

    td.print();

    SECTION("Appending same tabular data"){
        REQUIRE(td == td2);
    }



}
