module;
#include <cstring>
#include <vector>
#include <memory>
#include <iostream>

module atomix.mem.mem_route;

import atomix.mem.allocation_type;
import atomix.mem.aligned_allocator;
import atomix.bounds;
import atomix.config;

namespace atomix::mem::mem_route{
    /*
    namespace {

         [[nodiscard]] std::vector<Buffer> allocate_chunked(const size_t size, const size_t chunk_size, const size_t alignment = 64) {
            assert(size >= chunk_size);

            std::vector<Buffer> v;
            v.reserve((size / (chunk_size)) + 1);

            const size_t remainder = (size % (chunk_size));

            for (size_t i = 0; i < size / (chunk_size); ++i) {
                v.push_back(allocate_buffer(chunk_size, AllocationType::aligned_alloc, alignment));
            }
            if (remainder != 0){
                v.push_back(allocate_buffer( remainder, AllocationType::aligned_alloc, alignment));
            }
            return v;
        }

    }
    */


     [[nodiscard]] AllocInfo allocate(const size_t size, const size_t alignment ) {

        if (size < cut_size) {
            return {aligned_allocator::allocate(size, alignment), size, AllocationType::aligned_alloc};
        }
        else {
            return {aligned_allocator::allocate(size, alignment), size, AllocationType::aligned_alloc};
        }
    }


    void deallocate(uint8_t* ptr, const AllocationType alloc_t) {
        switch (alloc_t) {

            case AllocationType::aligned_alloc:
                aligned_allocator::deallocate(ptr);
                return;

            case AllocationType::none:
                return;

        }
    }

}



