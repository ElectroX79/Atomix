#include <cstdint>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "../external/catch2/catch_amalgamated.hpp"
#include "include/data_table_tester.hpp"
#include "data_table_printer.hpp"
#include "data_table.hpp"
#include "data_type.hpp"

namespace {
    atomix::DataTable make_four_column_table() {
        const std::vector<int32_t> ids{1, 2, 3};
        const std::vector<double> values{1.5, 2.5, 3.5};
        const std::vector<uint8_t> flags{1, 0, 1};
        const std::vector<std::string> names{"John", "Pork", "Call"};

        atomix::DataTable table;
        atomix::DataTableTester::artificial_append(table, ids, atomix::DataType::Int32, "ids");
        atomix::DataTableTester::artificial_append(table, values, atomix::DataType::Float64, "values");
        atomix::DataTableTester::artificial_append(table, flags, atomix::DataType::Bool, "flags");
        atomix::DataTableTester::artificial_append(table, names, atomix::DataType::List, "names");

        return table;
    }

    void check_column(
        const atomix::DataTable& table,
        const size_t index,
        const std::string_view expected_name,
        const atomix::DataType expected_type,
        const size_t expected_elements
    ) {
        CHECK(table.column_name(index) == expected_name);
        CHECK(table.column_datatype(index) == expected_type);
        CHECK(table.column_size(index) == expected_elements);
    }
}

TEST_CASE("DataTable basic representation", "[atomix::DataTable][Edge]") {
    atomix::DataTable td;

    std::vector<uint32_t> v1;
    std::vector<double> v2;
    std::vector<uint8_t> v3;

    std::vector<uint32_t> v1_aux{1, 2, 3};
    std::vector<double> v2_aux {1.1, 2.2, 3.3};
    std::vector<uint8_t> v3_aux{1, 0, 1};

    constexpr size_t n = 8000;
    v1.reserve(n*v1_aux.size());
    v2.reserve(n*v2_aux.size());
    v3.reserve(n*v3_aux.size());
    for (size_t i = 0; i < n; ++i) {
        v1.insert(v1.end(), v1_aux.begin(), v1_aux.end());
        v2.insert(v2.end(), v2_aux.begin(), v2_aux.end());
        v3.insert(v3.end(), v3_aux.begin(), v3_aux.end());
    }
    atomix::DataTableTester::artificial_append(td, v1, atomix::DataType::Int32, "test");
    atomix::DataTableTester::artificial_append(td, v2, atomix::DataType::Float64, "test2");
    atomix::DataTableTester::artificial_append(td, v3, atomix::DataType::Bool, "test3");

    SECTION("Data") {
        //atomix::DataTablePrinter::metadata(td);
        //atomix::DataTablePrinter::data(td);
    }
}

TEST_CASE("DataTable: DataTabla::at() integrity", "[atomix::DataTable][default]") {

    const atomix::DataTable td = make_four_column_table();

    CHECK(td.at<atomix::DataType::Int32>(0, 0) == 1);
    CHECK(td.at<atomix::DataType::Int32>(0, 1) == 2);
    CHECK(td.at<atomix::DataType::Int32>(0, 2) == 3);

    CHECK(td.at<atomix::DataType::Float64>(1, 0) == 1.5);
    CHECK(td.at<atomix::DataType::Float64>(1, 1) == 2.5);
    CHECK(td.at<atomix::DataType::Float64>(1, 2) == 3.5);

    CHECK(td.at<atomix::DataType::Bool>(2, 0) == true);
    CHECK(td.at<atomix::DataType::Bool>(2, 1) == false);
    CHECK(td.at<atomix::DataType::Bool>(2, 2) == true);


}

TEST_CASE("DataTable: empty table has no columns", "[atomix::DataTable][empty]") {
    const atomix::DataTable table;

    CHECK(table.n_columns() == 0);
}


TEST_CASE("DataTable: empty table supports zero-length extract and erase", "[atomix::DataTable][empty][range]") {
    const atomix::DataTable table;

    const atomix::DataTable extracted = table.extract(0, 0);
    const atomix::DataTable erased = table.erase(0, 0);

    CHECK(extracted.n_columns() == 0);
    CHECK(erased.n_columns() == 0);
}


TEST_CASE("DataTable: artificial append creates fixed-size columns with metadata", "[atomix::DataTable][metadata]") {
    const std::vector<int32_t> ids{1, 2, 3};
    const std::vector<double> values{1.5, 2.5, 3.5};
    const std::vector<uint8_t> flags{1, 0, 1};

    atomix::DataTable table;
    atomix::DataTableTester::artificial_append(table, ids, atomix::DataType::Int32, "ids");
    atomix::DataTableTester::artificial_append(table, values, atomix::DataType::Float64, "values");
    atomix::DataTableTester::artificial_append(table, flags, atomix::DataType::Bool, "flags");

    REQUIRE(table.n_columns() == 3);

    check_column(table, 0, "ids", atomix::DataType::Int32, ids.size());
    check_column(table, 1, "values", atomix::DataType::Float64, values.size());
    check_column(table, 2, "flags", atomix::DataType::Bool, flags.size());
}

TEST_CASE("DataTable: artificial append ignores empty vectors", "[atomix::DataTable][metadata][empty]") {
    const std::vector<int32_t> empty_ints;
    const std::vector<double> empty_doubles;
    const std::vector<uint8_t> empty_bools;

    atomix::DataTable table;
    atomix::DataTableTester::artificial_append(table, empty_ints, atomix::DataType::Int32, "empty_ints");
    atomix::DataTableTester::artificial_append(table, empty_doubles, atomix::DataType::Float64, "empty_doubles");
    atomix::DataTableTester::artificial_append(table, empty_bools, atomix::DataType::Bool, "empty_bools");

    CHECK(table.n_columns() == 0);
}


TEST_CASE("DataTable: extract can select all columns", "[atomix::DataTable][extract]") {
    const atomix::DataTable table = make_four_column_table();

    const atomix::DataTable extracted = table.extract(0, table.n_columns());

    REQUIRE(extracted.n_columns() == 4);

    check_column(extracted, 0, "ids", atomix::DataType::Int32, 3);
    check_column(extracted, 1, "values", atomix::DataType::Float64, 3);
    check_column(extracted, 2, "flags", atomix::DataType::Bool, 3);
    check_column(extracted, 3, "names", atomix::DataType::List, 3);
}

TEST_CASE("DataTable: extract can select prefix, middle, suffix, and empty ranges", "[atomix::DataTable][extract]") {
    const atomix::DataTable table = make_four_column_table();

    SECTION("prefix") {
        const atomix::DataTable extracted = table.extract(0, 2);

        REQUIRE(extracted.n_columns() == 2);
        check_column(extracted, 0, "ids", atomix::DataType::Int32, 3);
        check_column(extracted, 1, "values", atomix::DataType::Float64, 3);
    }

    SECTION("middle single column") {
        const atomix::DataTable extracted = table.extract(1, 2);

        REQUIRE(extracted.n_columns() == 1);
        check_column(extracted, 0, "values", atomix::DataType::Float64, 3);
    }

    SECTION("suffix") {
        const atomix::DataTable extracted = table.extract(2, 3);

        REQUIRE(extracted.n_columns() == 1);
        check_column(extracted, 0, "flags", atomix::DataType::Bool, 3);
    }

    SECTION("empty range at beginning") {
        const atomix::DataTable extracted = table.extract(0, 0);

        CHECK(extracted.n_columns() == 0);
    }

    SECTION("empty range at end") {
        const atomix::DataTable extracted = table.extract(table.n_columns(), table.n_columns());

        CHECK(extracted.n_columns() == 0);
    }
}

TEST_CASE("DataTable: erase can remove all columns", "[atomix::DataTable][erase]") {
    const atomix::DataTable table = make_four_column_table();

    const atomix::DataTable erased = table.erase(0, table.n_columns());

    CHECK(erased.n_columns() == 0);
}

TEST_CASE("DataTable: erase can remove prefix, middle, suffix, and empty ranges", "[atomix::DataTable][erase]") {
    const atomix::DataTable table = make_four_column_table();

    SECTION("prefix") {
        const atomix::DataTable erased = table.erase(0, 1);

        REQUIRE(erased.n_columns() == 3);
        check_column(erased, 0, "values", atomix::DataType::Float64, 3);
        check_column(erased, 1, "flags", atomix::DataType::Bool, 3);
        check_column(erased, 2, "names", atomix::DataType::List, 3);
    }

    SECTION("middle single column") {
        const atomix::DataTable erased = table.erase(1, 2);

        REQUIRE(erased.n_columns() == 3);
        check_column(erased, 0, "ids", atomix::DataType::Int32, 3);
        check_column(erased, 1, "flags", atomix::DataType::Bool, 3);
        check_column(erased, 2, "names", atomix::DataType::List, 3);
    }

    SECTION("suffix") {
        const atomix::DataTable erased = table.erase(2, 3);

        REQUIRE(erased.n_columns() == 3);
        check_column(erased, 0, "ids", atomix::DataType::Int32, 3);
        check_column(erased, 1, "values", atomix::DataType::Float64, 3);
        check_column(erased, 2, "names", atomix::DataType::List, 3);
    }

    SECTION("empty range at beginning keeps all columns") {
        const atomix::DataTable erased = table.erase(0, 0);

        REQUIRE(erased.n_columns() == 4);
        check_column(erased, 0, "ids", atomix::DataType::Int32, 3);
        check_column(erased, 1, "values", atomix::DataType::Float64, 3);
        check_column(erased, 2, "flags", atomix::DataType::Bool, 3);
        check_column(table, 3, "names", atomix::DataType::List, 3);
    }

    SECTION("empty range at end keeps all columns") {
        const atomix::DataTable erased = table.erase(table.n_columns(), table.n_columns());

        REQUIRE(erased.n_columns() == 4);
        check_column(erased, 0, "ids", atomix::DataType::Int32, 3);
        check_column(erased, 1, "values", atomix::DataType::Float64, 3);
        check_column(erased, 2, "flags", atomix::DataType::Bool, 3);
        check_column(erased, 3, "names", atomix::DataType::List, 3);
    }
}

TEST_CASE("DataTable: append combines columns in order", "[atomix::DataTable][append]") {
    const std::vector<int32_t> left_values{10, 20};
    const std::vector<double> right_values{3.14, 2.71};

    atomix::DataTable left;
    atomix::DataTable right;

    atomix::DataTableTester::artificial_append(left, left_values, atomix::DataType::Int32, "left_values");
    atomix::DataTableTester::artificial_append(right, right_values, atomix::DataType::Float64, "right_values");

    const atomix::DataTable appended = left.append(right);

    REQUIRE(appended.n_columns() == 2);
    check_column(appended, 0, "left_values", atomix::DataType::Int32, left_values.size());
    check_column(appended, 1, "right_values", atomix::DataType::Float64, right_values.size());
}

TEST_CASE("DataTable: append handles empty operands", "[atomix::DataTable][append][empty]") {
    const std::vector<int32_t> values{1, 2, 3};

    atomix::DataTable empty;
    atomix::DataTable non_empty;
    atomix::DataTableTester::artificial_append(non_empty, values, atomix::DataType::Int32, "values");

    SECTION("empty appended to empty") {
        const atomix::DataTable result = empty.append(empty);

        CHECK(result.n_columns() == 0);
    }

    SECTION("empty appended after non-empty") {
        const atomix::DataTable result = non_empty.append(empty);

        REQUIRE(result.n_columns() == 1);
        check_column(result, 0, "values", atomix::DataType::Int32, values.size());
    }

    SECTION("non-empty appended after empty") {
        const atomix::DataTable result = empty.append(non_empty);

        REQUIRE(result.n_columns() == 1);
        check_column(result, 0, "values", atomix::DataType::Int32, values.size());
    }
}

TEST_CASE("DataTable: append with self duplicates metadata", "[atomix::DataTable][append]") {
    const std::vector<int32_t> values{1, 2, 3};

    atomix::DataTable table;
    atomix::DataTableTester::artificial_append(table, values, atomix::DataType::Int32, "values");

    const atomix::DataTable result = table.append(table);

    REQUIRE(result.n_columns() == 2);
    check_column(result, 0, "values", atomix::DataType::Int32, values.size());
    check_column(result, 1, "values", atomix::DataType::Int32, values.size());
}

TEST_CASE("DataTable: copy constructor preserves table metadata", "[atomix::DataTable][copy]") {
    const atomix::DataTable original = make_four_column_table();

    const atomix::DataTable copied = original;

    REQUIRE(copied.n_columns() == 4);
    check_column(copied, 0, "ids", atomix::DataType::Int32, 3);
    check_column(copied, 1, "values", atomix::DataType::Float64, 3);
    check_column(copied, 2, "flags", atomix::DataType::Bool, 3);
    check_column(copied, 3, "names", atomix::DataType::List, 3);
}

TEST_CASE("DataTable: copy assignment preserves table metadata", "[atomix::DataTable][copy]") {
    const atomix::DataTable source = make_four_column_table();

    atomix::DataTable target;
    target = source;

    REQUIRE(target.n_columns() == 4);
    check_column(target, 0, "ids", atomix::DataType::Int32, 3);
    check_column(target, 1, "values", atomix::DataType::Float64, 3);
    check_column(target, 2, "flags", atomix::DataType::Bool, 3);
    check_column(target, 3, "names", atomix::DataType::List, 3);
}

TEST_CASE("DataTable: self copy assignment keeps table valid", "[atomix::DataTable][copy]") {
    atomix::DataTable table = make_four_column_table();

    table = table;

    REQUIRE(table.n_columns() == 4);
    check_column(table, 0, "ids", atomix::DataType::Int32, 3);
    check_column(table, 1, "values", atomix::DataType::Float64, 3);
    check_column(table, 2, "flags", atomix::DataType::Bool, 3);
    check_column(table, 3, "names", atomix::DataType::List, 3);
}

TEST_CASE("DataTable: move constructor transfers table metadata", "[atomix::DataTable][move]") {
    atomix::DataTable source = make_four_column_table();

    atomix::DataTable moved = std::move(source);

    REQUIRE(moved.n_columns() == 4);
    check_column(moved, 0, "ids", atomix::DataType::Int32, 3);
    check_column(moved, 1, "values", atomix::DataType::Float64, 3);
    check_column(moved, 2, "flags", atomix::DataType::Bool, 3);
    check_column(moved, 3, "names", atomix::DataType::List, 3);
}

TEST_CASE("DataTable: move assignment transfers table metadata", "[atomix::DataTable][move]") {
    atomix::DataTable source = make_four_column_table();

    atomix::DataTable target;
    target = std::move(source);

    REQUIRE(target.n_columns() == 4);
    check_column(target, 0, "ids", atomix::DataType::Int32, 3);
    check_column(target, 1, "values", atomix::DataType::Float64, 3);
    check_column(target, 2, "flags", atomix::DataType::Bool, 3);
    check_column(target, 3, "names", atomix::DataType::List, 3);
}

TEST_CASE("DataTable: int32 numeric limits are accepted", "[atomix::DataTable][limits]") {
    const std::vector<int32_t> values{
        std::numeric_limits<int32_t>::min(),
        -1,
        0,
        1,
        std::numeric_limits<int32_t>::max()
    };

    atomix::DataTable table;
    atomix::DataTableTester::artificial_append(table, values, atomix::DataType::Int32, "int32_limits");

    REQUIRE(table.n_columns() == 1);
    check_column(table, 0, "int32_limits", atomix::DataType::Int32, values.size());

    CHECK_NOTHROW(atomix::DataTablePrinter::metadata(table));
    CHECK_NOTHROW(atomix::DataTablePrinter::data(table));
}

TEST_CASE("DataTable: float64 special values are accepted", "[atomix::DataTable][limits]") {
    const std::vector<double> values{
        -std::numeric_limits<double>::infinity(),
        std::numeric_limits<double>::lowest(),
        -0.0,
        0.0,
        std::numeric_limits<double>::min(),
        std::numeric_limits<double>::max(),
        std::numeric_limits<double>::infinity(),
        std::numeric_limits<double>::quiet_NaN()
    };

    atomix::DataTable table;
    atomix::DataTableTester::artificial_append(table, values, atomix::DataType::Float64, "float64_limits");

    REQUIRE(table.n_columns() == 1);
    check_column(table, 0, "float64_limits", atomix::DataType::Float64, values.size());

    CHECK_NOTHROW(atomix::DataTablePrinter::metadata(table));
    CHECK_NOTHROW(atomix::DataTablePrinter::data(table));
}

TEST_CASE("DataTable: bool byte values are accepted", "[atomix::DataTable][limits]") {
    const std::vector<uint8_t> values{
        0,
        1,
        std::numeric_limits<uint8_t>::max()
    };

    atomix::DataTable table;
    atomix::DataTableTester::artificial_append(table, values, atomix::DataType::Bool, "bool_bytes");

    REQUIRE(table.n_columns() == 1);
    check_column(table, 0, "bool_bytes", atomix::DataType::Bool, values.size());

    CHECK_NOTHROW(atomix::DataTablePrinter::metadata(table));
    CHECK_NOTHROW(atomix::DataTablePrinter::data(table));
}

TEST_CASE("DataTable: large columns can span multiple buffers", "[atomix::DataTable][large][buffer]") {
    std::vector<int32_t> values;
    constexpr size_t repeat_count = 8000;

    values.reserve(repeat_count * 3);
    for (size_t i = 0; i < repeat_count; ++i) {
        values.push_back(1);
        values.push_back(2);
        values.push_back(3);
    }

    atomix::DataTable table;
    atomix::DataTableTester::artificial_append(table, values, atomix::DataType::Int32, "large_int32");

    REQUIRE(table.n_columns() == 1);
    check_column(table, 0, "large_int32", atomix::DataType::Int32, values.size());

    CHECK_NOTHROW(atomix::DataTablePrinter::metadata(table));
}

TEST_CASE("DataTable: composed extract, erase, and append preserve resulting order", "[atomix::DataTable][composition]") {
    const atomix::DataTable table = make_four_column_table();

    const atomix::DataTable only_ids = table.extract(0, 1);
    const atomix::DataTable without_ids = table.erase(0, 1);
    const atomix::DataTable recombined = without_ids.append(only_ids);

    REQUIRE(recombined.n_columns() == 4);
    check_column(recombined, 0, "values", atomix::DataType::Float64, 3);
    check_column(recombined, 1, "flags", atomix::DataType::Bool, 3);
    check_column(recombined, 2, "names", atomix::DataType::List, 3);
    check_column(recombined, 3, "ids", atomix::DataType::Int32, 3);

}