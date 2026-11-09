module;
#include <cstdint>


export module atomix.mem.allocation_type;
import atomix.mem.vmem;

export namespace atomix::mem {

    enum class AllocationType : uint8_t {
        aligned_alloc,
        none,
    };
}



