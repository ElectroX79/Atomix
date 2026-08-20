
#ifndef MEM_ROUTE_HPP
#define MEM_ROUTE_HPP

#include <vector>
#include <memory>

#include "allocation_type.hpp"


namespace atomix::mem{

    enum class CopyType: uint8_t {
        Deep,
        Shallow,
        Reinitialize
    };

    class Buffer;

    namespace mem_route {
        // Future change: custom allocator, see documentation

         [[nodiscard]] std::vector<std::shared_ptr<Buffer>> allocate( size_t size, size_t& chunk_size,size_t alignment = 64);
        void deallocate(uint8_t* ptr, AllocationType alloc_t);

        [[nodiscard]] std::vector<std::shared_ptr<Buffer>> copy( const std::vector<std::shared_ptr<Buffer>>& ref, const std::vector<CopyType>& copy_types);

    }

}

#endif


