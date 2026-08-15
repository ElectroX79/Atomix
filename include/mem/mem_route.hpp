
#ifndef MEM_ROUTE_HPP
#define MEM_ROUTE_HPP

#include <vector>
#include <memory>

#include "allocation_type.hpp"

namespace atomix::mem{
    class Buffer;
    namespace mem_route {
        // Future change: custom allocator, see documentation

        std::vector<std::shared_ptr<Buffer>> allocate(const size_t size, size_t& chunk_size, const size_t alignment = 64);
        void deallocate(uint8_t* ptr, AllocationType alloc_t);

    }

}

#endif


