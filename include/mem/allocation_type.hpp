#ifndef ALLOCATION_TYPE
#define ALLOCATION_TYPE

#include <cstdint>
#include <atomic>
#include <span>

#include "vmem.hpp"

namespace atomix::mem {

    enum class AllocationType : uint8_t {
        aligned_alloc,
        none,
    };
}



#endif
