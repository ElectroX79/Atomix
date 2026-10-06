

#include <cstdint>
#include <format>
#include <vector>
#include <span>
#include <cstdint>

import atomix.data.data_table;
import atomix.mem.loader;
import atomix.mem.unloader;
import atomix.data.data_type;

using int32_dt = atomix::data::type_of_t<atomix::data::DataType::Int32>;
using float_dt = atomix::data::type_of_t<atomix::data::DataType::Float64>;
using char_dt = atomix::data::type_of_t<atomix::data::DataType::Char>;

int main() {

    const auto dt = atomix::mem::loader::initialize_table("try1", "try1var");
    auto dt2 = dt.clone("try1_1");
}
