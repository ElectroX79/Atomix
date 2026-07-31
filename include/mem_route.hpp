
#ifndef MEM_ROUTE_HPP
#define MEM_ROUTE_HPP

#include <vector>
#include <memory>

#include "cassert"
#include "vmem.hpp"
#include "aligned_allocator.hpp"
#include "allocation_type.hpp"

namespace atomix::mem{
    class Buffer;
    namespace mem_route {
        /*
          [[deprecated]] static std::vector<std::shared_ptr<Buffer>> allocate_chunked2(size_t size, const size_t chunk_size) {
            std::vector<std::shared_ptr<Buffer>> v;
            v.reserve((size / (chunk_size)) + 1);

            for (size_t i = 0; i < size / (chunk_size); ++i) {
                uint8_t* ptr = mem::vmem::allocate(chunk_size);
                v.push_back(std::make_shared<Buffer>(ptr, chunk_size, AllocationType::page));
            }

            if (size % (chunk_size) != 0){
                uint8_t* ptr = mem::vmem::allocate(size % (chunk_size) );
                v.push_back(std::make_shared<Buffer>(ptr,(size % (chunk_size)), AllocationType::page));
            }
            return v;
        }
        */

        std::vector<std::shared_ptr<Buffer>> allocate_chunked(const size_t size, const size_t chunk_size);

        std::vector<std::shared_ptr<Buffer>> allocate(const size_t size, const size_t alignment = 64);


        void deallocate(uint8_t* ptr, const size_t size, const AllocationType type, VmemHandler* vmem_handler);
    }

}

#endif


