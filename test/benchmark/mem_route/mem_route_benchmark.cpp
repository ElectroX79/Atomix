#include <cstdint>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "../external/catch2/catch_amalgamated.hpp"
#include "../../include/data_table_tester.hpp"
#include "../include/data/data_table.hpp"
#include "../include/data/data_type.hpp"



namespace {
    atomix::DataTable make_three_column_table(const size_t multiplier) {

        const std::vector<int32_t> ids{1, 2, 3};
        const std::vector<double> values{1.5, 2.5, 3.5};
        const std::vector<char> flags{'A', 'B', 'C'};

        std::vector<int32_t> ids2{};
        std::vector<double> values2{};
        std::vector<char> flags2{};

        for (size_t i = 0; i < multiplier; ++i) {
            ids2.insert(ids2.end(), ids.begin(), ids.end());
            values2.insert(values2.end(), values.begin(), values.end());
            flags2.insert(flags2.end(), flags.begin(), flags.end());
        }

        atomix::DataTable table;
        atomix::DataTableTester::artificial_append_fixed<atomix::DataType::Int32>(table, ids2,"ids");
        atomix::DataTableTester::artificial_append_fixed<atomix::DataType::Float64>(table, values2, "values");
        atomix::DataTableTester::artificial_append_fixed<atomix::DataType::Char>(table, flags2, "flags");
        return table;
    }

    void sequential_access(const size_t mult, const atomix::DataTable& table ) {
        const size_t n_elements_column = 3*mult;
        for (size_t x = 0; x < n_elements_column; ++x) {
            auto temp = table.at<atomix::DataType::Int32>(0,x);
            Catch::Benchmark::deoptimize_value(temp);
        }

        for (size_t x = 0; x < n_elements_column; ++x) {
            auto temp = table.at<atomix::DataType::Float64>(1,x);
            Catch::Benchmark::deoptimize_value(temp);
        }

        for (size_t x = 0; x < n_elements_column; ++x) {
            auto temp = table.at<atomix::DataType::Char>(2,x);
            Catch::Benchmark::deoptimize_value(temp);
        }
    }
}


TEST_CASE("Small size, alloc + dealloc", "[mult_256][alloc + dealloc]") {
    constexpr size_t mult = 256;

    constexpr size_t repeat = 1;

    BENCHMARK("Multiple alloc + dealloc") {
        for (size_t i = 0; i < repeat; ++i) {
            atomix::DataTable table = make_three_column_table(mult);
        }
    };
}

TEST_CASE("Small size, access", "[mult_256][access]") {
    constexpr size_t mult = 256;

    const atomix::DataTable table = make_three_column_table(mult);
    BENCHMARK("Access") {
        sequential_access(mult, table);
    };

}


//

TEST_CASE("Medium size, alloc + dealloc", "[mult_8k][alloc + dealloc]") {
    constexpr size_t k = 1024;
    constexpr size_t mult = 8*k;

    constexpr size_t repeat = 1;

    BENCHMARK("Multiple alloc + dealloc") {
        for (size_t i = 0; i < repeat; ++i) {
            atomix::DataTable table = make_three_column_table(mult);
        }
    };
}

TEST_CASE("Medium size, access", "[mult_8k][access]") {
    constexpr size_t k = 1024;
    constexpr size_t mult = 8*k;

    const atomix::DataTable table = make_three_column_table(mult);
    BENCHMARK("Access") {
        sequential_access(mult, table);
    };

}

//


TEST_CASE("Big size, alloc + dealloc", "[mult_128k][alloc + dealloc]") {
    constexpr size_t k = 1024;
    constexpr size_t mult = 128*k;

    constexpr size_t repeat = 1;

    BENCHMARK("Multiple alloc + dealloc") {
        for (size_t i = 0; i < repeat; ++i) {
            atomix::DataTable table = make_three_column_table(mult);
        }
    };

}

TEST_CASE("Big size, access", "[mult_128k][access]") {
    constexpr size_t k = 1024;
    constexpr size_t mult = 128*k;

    const atomix::DataTable table = make_three_column_table(mult);
    BENCHMARK("Access") {
        sequential_access(mult, table);
    };

}

//


TEST_CASE("Huge size, alloc + dealloc", "[mult_8m][alloc + dealloc]") {
    constexpr size_t k = 1024;
    constexpr size_t mult = 8*k*k;

    constexpr size_t repeat = 1;

    BENCHMARK("Multiple alloc + dealloc") {
        for (size_t i = 0; i < repeat; ++i) {
            atomix::DataTable table = make_three_column_table(mult);
        }
    };

}

TEST_CASE("Huge size, access", "[mult_8m][access]") {
    constexpr size_t k = 1024;
    constexpr size_t mult = 8*k*k;

    const atomix::DataTable table = make_three_column_table(mult);
    BENCHMARK("Access") {
        sequential_access(mult, table);
    };

}


