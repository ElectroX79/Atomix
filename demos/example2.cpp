

#include <format>
#include <iostream>
#include <ostream>


import atomix.data.data_table;
import atomix.mem.loader;
import atomix.mem.unloader;
import atomix.data.data_type;
import atomix.data.select_proxy;

int main() {

    auto dt = atomix::mem::loader::initialize_table("try1");
    auto dt_clone = dt.clone("try1_clone");

    atomix::data::SelectProxy proxy_dt = atomix::data::SelectProxy(dt);
    atomix::data::SelectProxy proxy_dt_clone = atomix::data::SelectProxy(dt_clone);

    proxy_dt.print_result();
    std::cout << "second: " << std::endl;
    proxy_dt_clone.print_result();
}
