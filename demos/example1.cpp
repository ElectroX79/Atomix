

#include <format>
#include <vector>
#include <span>
#include <iostream>
#include <ostream>
#include <chrono>

import atomix.data.data_table;
import atomix.mem.loader;
import atomix.mem.unloader;
import atomix.data.data_type;
import atomix.data.select_proxy;

using int32_dt = atomix::data::int32_dt;
using float_dt = atomix::data::float64_dt;
using char_dt = atomix::data::char_dt;

int main() {
    atomix::data::DataTable dt = atomix::mem::loader::initialize_table_clean("try1");


    //Note: for primitive types atomix::data::DataType::Undefined is totally optional
    dt.set_new_column(
        "integer1",
        atomix::data::DataType::Int32,
        atomix::data::DataType::Undefined
    );

    dt.set_new_column(
        "float1",
        atomix::data::DataType::Float64,
        atomix::data::DataType::Undefined
    );

    dt.set_new_column("char1",
        atomix::data::DataType::Char,
        atomix::data::DataType::Undefined
    );

    //size_t n = 1024 * 1024 * 100;
    size_t n = 1024 * 1024; //change if needed
    std::vector<int32_dt> int32_vec_rep{1, 2, 3, 4, 5};
    std::vector<float_dt> float64_vec_rep{1.1, 2.2, 3.3, 4.4, 5.5};
    std::vector<char_dt> char_vec_rep{'a', 'b', 'c', 'd', 'e'};

    std::vector<int32_dt> int32_vec;
    std::vector<float_dt> float64_vec;
    std::vector<char_dt> char_vec;

    int32_vec.reserve(n * 5);
    float64_vec.reserve(n * 5);
    char_vec.reserve(n * 5);

    for (size_t i = 0; i < n; i++) {
        int32_vec.insert(int32_vec.end(), int32_vec_rep.begin(), int32_vec_rep.end());
        float64_vec.insert(float64_vec.end(), float64_vec_rep.begin(), float64_vec_rep.end());
        char_vec.insert(char_vec.end(), char_vec_rep.begin(), char_vec_rep.end());
    }



    const std::span int32_sp = int32_vec;
    const std::span float64_sp = float64_vec;
    const std::span char_sp = char_vec;


    const std::vector<atomix::data::TGroupColumn> columns{int32_sp, float64_sp, char_sp};

    dt.add_batch(columns);

    const atomix::data::SelectProxy proxy (dt);
    
    proxy.print_result();

    const auto t_start1 = std::chrono::steady_clock::now();
    const auto sum1 = proxy.col_sum<atomix::data::DataType::Int32>(0);
    const auto t_end1 = std::chrono::steady_clock::now();

    const auto t_start2 = std::chrono::steady_clock::now();
    const auto sum2 = proxy.col_sum<atomix::data::DataType::Float64>(1);
    const auto t_end2 = std::chrono::steady_clock::now();

    const auto interval1_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(t_end1 - t_start1).count();
    const auto interval2_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(t_end2 - t_start2).count();


    std::cout << "interval1_ns: " << interval1_ns << std::endl;
    std::cout << "interval2_ns: " << interval2_ns << std::endl;


    std::cout << "sum1: " << sum1 << std::endl;
    std::cout << "sum2: " << sum2 << std::endl;
}
