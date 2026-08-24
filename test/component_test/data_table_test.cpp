#include <cstdint>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include <ranges>

#include "../external/catch2/catch_amalgamated.hpp"
#include "../include/data_table_tester.hpp"
#include "../include/data/data_table_printer.hpp"
#include "../include/data/data_table.hpp"
#include "../include/data/data_type.hpp"

namespace {
    atomix::DataTable make_four_column_table() {
        const std::vector<int32_t> ids{1, 2, 3};
        const std::vector<double> values{1.5, 2.5, 3.5};
        const std::vector<char> flags{'A', 'B', 'C'};
        std::vector<char> names{'J', 'o', 'h', 'n', 'P', 'o', 'r', 'k', 'C', 'a', 'l', 'l'};
        const std::span sp{names};
        const std::vector<uint32_t> offsets{0, 4, 8};

        atomix::DataTable table;
        atomix::DataTableTester::artificial_append_fixed<atomix::DataType::Int32>(table, ids,"ids");
        atomix::DataTableTester::artificial_append_fixed<atomix::DataType::Float64>(table, values, "values");
        atomix::DataTableTester::artificial_append_fixed<atomix::DataType::Char>(table, flags, "flags");
        atomix::DataTableTester::artificial_append_variable<atomix::DataType::List, atomix::DataType::Char>(table, sp, "names", offsets );
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

// TODO: add more specifics TEST_CASE for DataType::List
// TODO: add some tester for the printers

TEST_CASE("DataTable: empty table has no columns", "[atomix::DataTable][empty]") {
    const atomix::DataTable table;

    CHECK(table.n_columns() == 0);
}


TEST_CASE("DataTable: empty table supports zero-length extract and erase", "[atomix::DataTable][empty][range]") {
    const atomix::DataTable table;

    const atomix::DataTable extracted = table.extract(0, 0);
    atomix::DataTable erased = table;
    erased.erase(0, 0);

    CHECK(extracted.n_columns() == 0);
    CHECK(erased.n_columns() == 0);
}


TEST_CASE("DataTable: artificial append creates fixed-size columns with metadata", "[atomix::DataTable][metadata]") {
    const std::vector<int32_t> ids{1, 2, 3};
    const std::vector<double> values{1.5, 2.5, 3.5};
    const std::vector<char> flags{'A', 'B', 'C'};
    std::vector<char> names{'J', 'o', 'h', 'n', 'P', 'o', 'r', 'k', 'C', 'a', 'l', 'l'};
    const std::span sp{names};
    const std::vector<uint32_t> offsets{0, 4, 8};

    atomix::DataTable table;
    atomix::DataTableTester::artificial_append_fixed<atomix::DataType::Int32>(table, ids,"ids");
    atomix::DataTableTester::artificial_append_fixed<atomix::DataType::Float64>(table, values, "values");
    atomix::DataTableTester::artificial_append_fixed<atomix::DataType::Char>(table, flags, "flags");
    atomix::DataTableTester::artificial_append_variable<atomix::DataType::List, atomix::DataType::Char>(table, sp, "names", offsets );

    REQUIRE(table.n_columns() == 4);

    check_column(table, 0, "ids", atomix::DataType::Int32, ids.size());
    check_column(table, 1, "values", atomix::DataType::Float64, values.size());
    check_column(table, 2, "flags", atomix::DataType::Char, flags.size());
}

TEST_CASE("DataTable: artificial append ignores empty vectors", "[atomix::DataTable][metadata][empty]") {
    const std::vector<int32_t> ids;
    const std::vector<double> values;
    const std::vector<char> flags;
    std::vector<char> names;
    const std::span<char> sp{names};
    const std::vector<uint32_t> offsets{0, 4, 8};

    atomix::DataTable table;
    atomix::DataTableTester::artificial_append_fixed<atomix::DataType::Int32>(table, ids,"ids");
    atomix::DataTableTester::artificial_append_fixed<atomix::DataType::Float64>(table, values, "values");
    atomix::DataTableTester::artificial_append_fixed<atomix::DataType::Char>(table, flags, "flags");
    atomix::DataTableTester::artificial_append_variable<atomix::DataType::List, atomix::DataType::Char>(table, sp, "names", offsets );

    CHECK(table.n_columns() == 0);
}

TEST_CASE("DataTable: DataTabla::at() integrity", "[atomix::DataTable][default]") {

    const atomix::DataTable td = make_four_column_table();

    CHECK(td.at<atomix::DataType::Int32>(0, 0) == 1);
    CHECK(td.at<atomix::DataType::Int32>(0, 1) == 2);
    CHECK(td.at<atomix::DataType::Int32>(0, 2) == 3);

    CHECK(td.at<atomix::DataType::Float64>(1, 0) == 1.5);
    CHECK(td.at<atomix::DataType::Float64>(1, 1) == 2.5);
    CHECK(td.at<atomix::DataType::Float64>(1, 2) == 3.5);

    CHECK(td.at<atomix::DataType::Char>(2, 0) == 'A');
    CHECK(td.at<atomix::DataType::Char>(2, 1) == 'B');
    CHECK(td.at<atomix::DataType::Char>(2, 2) == 'C');


    /*
    std::vector<char> name1{'J', 'o', 'h', 'n'};
    const std::span sp1{name1};

    std::vector<char> name2{'P', 'o', 'r', 'k'};
    const std::span sp2{name2};

    std::vector<char> name3{'C', 'a', 'l', 'l'};
    const std::span sp3{name3};


    CHECK(std::ranges::equal(td.at_list<atomix::DataType::Char>(2, 0), sp1));
    CHECK(std::ranges::equal(td.at_list<atomix::DataType::Char>(2, 1), sp2));
    CHECK(std::ranges::equal(td.at_list<atomix::DataType::Char>(2, 2), sp3));
    */
    //TODO: Decomment when DataTable::at_list is ready
}


TEST_CASE("DataTable: extract can select all columns", "[atomix::DataTable][extract]") {
    const atomix::DataTable table = make_four_column_table();

    const atomix::DataTable extracted = table.extract(0, table.n_columns());

    REQUIRE(extracted.n_columns() == 4);

    check_column(extracted, 0, "ids", atomix::DataType::Int32, 3);
    check_column(extracted, 1, "values", atomix::DataType::Float64, 3);
    check_column(extracted, 2, "flags", atomix::DataType::Char, 3);
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
        check_column(extracted, 0, "flags", atomix::DataType::Char, 3);
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
    atomix::DataTable table = make_four_column_table();

    table.erase(0, table.n_columns());

    CHECK(table.n_columns() == 0);
}

TEST_CASE("DataTable: erase can remove prefix, middle, suffix, and empty ranges", "[atomix::DataTable][erase]") {
    atomix::DataTable table = make_four_column_table();

    SECTION("prefix") {
        table.erase(0, 1);

        REQUIRE(table.n_columns() == 3);
        check_column(table, 0, "values", atomix::DataType::Float64, 3);
        check_column(table, 1, "flags", atomix::DataType::Char, 3);
        check_column(table, 2, "names", atomix::DataType::List, 3);
    }

    SECTION("middle single column") {
        table.erase(1, 2);

        REQUIRE(table.n_columns() == 3);
        check_column(table, 0, "ids", atomix::DataType::Int32, 3);
        check_column(table, 1, "flags", atomix::DataType::Char, 3);
        check_column(table, 2, "names", atomix::DataType::List, 3);
    }

    SECTION("suffix") {
        table.erase(2, 3);

        REQUIRE(table.n_columns() == 3);
        check_column(table, 0, "ids", atomix::DataType::Int32, 3);
        check_column(table, 1, "values", atomix::DataType::Float64, 3);
        check_column(table, 2, "names", atomix::DataType::List, 3);
    }

    SECTION("empty range at beginning keeps all columns") {
        table.erase(0, 0);

        REQUIRE(table.n_columns() == 4);
        check_column(table, 0, "ids", atomix::DataType::Int32, 3);
        check_column(table, 1, "values", atomix::DataType::Float64, 3);
        check_column(table, 2, "flags", atomix::DataType::Char, 3);
        check_column(table, 3, "names", atomix::DataType::List, 3);
    }

    SECTION("empty range at end keeps all columns") {
        table.erase(table.n_columns(), table.n_columns());

        REQUIRE(table.n_columns() == 4);
        check_column(table, 0, "ids", atomix::DataType::Int32, 3);
        check_column(table, 1, "values", atomix::DataType::Float64, 3);
        check_column(table, 2, "flags", atomix::DataType::Char, 3);
        check_column(table, 3, "names", atomix::DataType::List, 3);
    }
}

TEST_CASE("DataTable: append combines columns in order", "[atomix::DataTable][append]") {
    const std::vector<int32_t> left_values{10, 20};
    const std::vector<double> right_values{3.14, 2.71};

    atomix::DataTable left;
    atomix::DataTable right;

    atomix::DataTableTester::artificial_append_fixed<atomix::DataType::Int32>(left, left_values, "left_values");
    atomix::DataTableTester::artificial_append_fixed<atomix::DataType::Float64>(right, right_values, "right_values");

    left.append(right);

    REQUIRE(left.n_columns() == 2);
    check_column(left, 0, "left_values", atomix::DataType::Int32, left_values.size());
    check_column(left, 1, "right_values", atomix::DataType::Float64, right_values.size());
}

TEST_CASE("DataTable: append handles empty operands", "[atomix::DataTable][append][empty]") {
    const std::vector<int32_t> values{1, 2, 3};

    atomix::DataTable empty;
    atomix::DataTable non_empty;
    atomix::DataTableTester::artificial_append_fixed<atomix::DataType::Int32>(non_empty, values,  "values");

    SECTION("empty appended to empty") {
        empty.append(empty);

        CHECK(empty.n_columns() == 0);
    }

    SECTION("empty appended after non-empty") {
        non_empty.append(empty);

        REQUIRE(non_empty.n_columns() == 1);
        check_column(non_empty, 0, "values", atomix::DataType::Int32, values.size());
    }

    SECTION("non-empty appended after empty") {
        empty.append(non_empty);

        REQUIRE(empty.n_columns() == 1);
        check_column(empty, 0, "values", atomix::DataType::Int32, values.size());
    }
}

TEST_CASE("DataTable: append with self duplicates metadata", "[atomix::DataTable][append]") {
    const std::vector<int32_t> values{1, 2, 3};

    atomix::DataTable table;
    atomix::DataTableTester::artificial_append_fixed<atomix::DataType::Int32>(table, values,  "values");
    table.append(table);

    REQUIRE(table.n_columns() == 2);
    check_column(table, 0, "values", atomix::DataType::Int32, values.size());
    check_column(table, 1, "values", atomix::DataType::Int32, values.size());
}

TEST_CASE("DataTable: copy constructor preserves table metadata", "[atomix::DataTable][copy]") {
    const atomix::DataTable original = make_four_column_table();

    const atomix::DataTable copied = original;

    REQUIRE(copied.n_columns() == 4);
    check_column(copied, 0, "ids", atomix::DataType::Int32, 3);
    check_column(copied, 1, "values", atomix::DataType::Float64, 3);
    check_column(copied, 2, "flags", atomix::DataType::Char, 3);
    check_column(copied, 3, "names", atomix::DataType::List, 3);
}

TEST_CASE("DataTable: copy assignment preserves table metadata", "[atomix::DataTable][copy]") {
    const atomix::DataTable source = make_four_column_table();

    atomix::DataTable target;
    target = source;

    REQUIRE(target.n_columns() == 4);
    check_column(target, 0, "ids", atomix::DataType::Int32, 3);
    check_column(target, 1, "values", atomix::DataType::Float64, 3);
    check_column(target, 2, "flags", atomix::DataType::Char, 3);
    check_column(target, 3, "names", atomix::DataType::List, 3);
}

TEST_CASE("DataTable: self copy assignment keeps table valid", "[atomix::DataTable][copy]") {
    atomix::DataTable table = make_four_column_table();

    table = table;

    REQUIRE(table.n_columns() == 4);
    check_column(table, 0, "ids", atomix::DataType::Int32, 3);
    check_column(table, 1, "values", atomix::DataType::Float64, 3);
    check_column(table, 2, "flags", atomix::DataType::Char, 3);
    check_column(table, 3, "names", atomix::DataType::List, 3);
}

TEST_CASE("DataTable: move constructor transfers table metadata", "[atomix::DataTable][move]") {
    atomix::DataTable source = make_four_column_table();

    atomix::DataTable moved = std::move(source);

    REQUIRE(moved.n_columns() == 4);
    check_column(moved, 0, "ids", atomix::DataType::Int32, 3);
    check_column(moved, 1, "values", atomix::DataType::Float64, 3);
    check_column(moved, 2, "flags", atomix::DataType::Char, 3);
    check_column(moved, 3, "names", atomix::DataType::List, 3);
}

TEST_CASE("DataTable: move assignment transfers table metadata", "[atomix::DataTable][move]") {
    atomix::DataTable source = make_four_column_table();

    atomix::DataTable target;
    target = std::move(source);

    REQUIRE(target.n_columns() == 4);
    check_column(target, 0, "ids", atomix::DataType::Int32, 3);
    check_column(target, 1, "values", atomix::DataType::Float64, 3);
    check_column(target, 2, "flags", atomix::DataType::Char, 3);
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
    atomix::DataTableTester::artificial_append_fixed<atomix::DataType::Int32>(table, values,  "int32_limits");

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
    atomix::DataTableTester::artificial_append_fixed<atomix::DataType::Float64>(table, values,  "float64_limits");

    REQUIRE(table.n_columns() == 1);
    check_column(table, 0, "float64_limits", atomix::DataType::Float64, values.size());

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
    atomix::DataTableTester::artificial_append_fixed<atomix::DataType::Int32>(table, values, "large_int32");

    REQUIRE(table.n_columns() == 1);
    check_column(table, 0, "large_int32", atomix::DataType::Int32, values.size());

    CHECK_NOTHROW(atomix::DataTablePrinter::metadata(table));
}

TEST_CASE("DataTable: composed extract, erase, and append preserve resulting order", "[atomix::DataTable][composition]") {
    const atomix::DataTable table = make_four_column_table();

    const atomix::DataTable only_ids = table.extract(0, 1);


    atomix::DataTable without_ids = table;
    without_ids.erase(0, 1);

    atomix::DataTable recombined = without_ids;
    recombined.append(only_ids);

    REQUIRE(recombined.n_columns() == 4);
    check_column(recombined, 0, "values", atomix::DataType::Float64, 3);
    check_column(recombined, 1, "flags", atomix::DataType::Char, 3);
    check_column(recombined, 2, "names", atomix::DataType::List, 3);
    check_column(recombined, 3, "ids", atomix::DataType::Int32, 3);

}

/*

const atomix::DataTable table = make_four_column_table();

const atomix::DataTable only_ids = table.extract(0, 1);
const atomix::DataTable without_ids = table.erase(0, 1);
const atomix::DataTable recombined = without_ids.append(only_ids);
*/