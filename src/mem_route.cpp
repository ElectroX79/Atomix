#include <vector>
#include <memory>

#include "cassert"
#include "mem/aligned_allocator.hpp"
#include "mem/buffer.hpp"
#include "mem/allocation_type.hpp"

namespace atomix::mem::mem_route{

    std::vector<std::shared_ptr<Buffer>> allocate_chunked(const size_t size, const size_t chunk_size) {
        assert(size >= chunk_size);

        std::vector<std::shared_ptr<Buffer>> v;
        v.reserve((size / (chunk_size)) + 1);

        const size_t remainder = (size % (chunk_size));

        for (size_t i = 0; i < size / (chunk_size); ++i) {
            uint8_t* ptr = aligned_allocator::allocate(chunk_size);
            v.push_back(std::make_shared<Buffer>(ptr, chunk_size));
        }
        if (remainder != 0){
            uint8_t* ptr = aligned_allocator::allocate(remainder);
            v.push_back(std::make_shared<Buffer>(ptr, remainder));
        }
        return v;
    }

    std::vector<std::shared_ptr<Buffer>> allocate(const size_t size, size_t& chunk_size, const size_t alignment) {
        constexpr uint64_t kb = 1024;
        constexpr uint64_t paged_min = 64 * kb;

        if (size < paged_min) {
            uint8_t* ptr = aligned_allocator::allocate(size, alignment);
            chunk_size = 0;
            return std::vector<std::shared_ptr<Buffer>>{std::make_shared<Buffer> (ptr, size)};
        }

        chunk_size = 64 * kb;
        return allocate_chunked(size, chunk_size);
    }


    void deallocate(uint8_t* ptr) {
        aligned_allocator::deallocate(ptr);
    }



}



