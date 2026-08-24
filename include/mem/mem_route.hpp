
#ifndef MEM_ROUTE_HPP
#define MEM_ROUTE_HPP

#include <vector>
#include <memory>
#include "buffer.hpp"

#include "allocation_type.hpp"


namespace atomix::mem{
    /*
    enum class CopyType: uint8_t {
        Deep,
        Shallow,
        Reinitialize
    };
    */


    namespace mem_route {
        // Future change: custom allocator, see documentation

        [[nodiscard]] std::vector<Buffer> allocate( size_t size, size_t& chunk_size,size_t alignment = 64);
        void deallocate(uint8_t* ptr, AllocationType alloc_t);

        Buffer allocate_buffer(size_t size, AllocationType alloc_t, size_t alignment = 64);

       // [[nodiscard]] std::vector<std::shared_ptr<Buffer>> copy( const std::vector<std::shared_ptr<Buffer>>& ref, const std::vector<CopyType>& copy_types);

    }

}

#endif


