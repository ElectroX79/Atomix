#include <vector>
#include <memory>

#include "cassert"
#include "vmem.hpp"
#include "aligned_allocator.hpp"
#include "buffer.hpp"
#include "allocation_type.hpp"

namespace atomix::mem::mem_route{
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

    std::vector<std::shared_ptr<Buffer>> allocate_chunked(const size_t size, const size_t chunk_size) {
        assert(size >= chunk_size);
        assert(chunk_size % 4096 == 0);

        std::vector<std::shared_ptr<Buffer>> v;
        v.reserve((size / (chunk_size)) + 1);
        uint8_t* ptr = mem::vmem::allocate(size);
        const bool extra = (size % (chunk_size) != 0);
        auto vmh_ptr = new VmemHandler(ptr, size, size / (chunk_size) + extra );

        for (size_t i = 0; i < size / (chunk_size); ++i) {
            v.push_back(std::make_shared<Buffer>(ptr + (chunk_size * i), chunk_size, AllocationType::chunked_page, vmh_ptr));
        }
        if (extra){
            v.push_back(std::make_shared<Buffer>(ptr + size - (size % chunk_size),(size % chunk_size), AllocationType::chunked_page, vmh_ptr));
        }
        return v;
    }

    std::vector<std::shared_ptr<Buffer>> allocate(const size_t size, const size_t alignment) {
        constexpr uint64_t kb = 1024;
        constexpr uint64_t mb = 1024 * kb;

        constexpr uint64_t paged_min = 16 * kb;
        constexpr uint64_t paged_chunked_min = 128 * kb; //chunks of 64 kb
        constexpr uint64_t paged_big_chunked_min = 256 * mb; //chunks of 1mb


        if (size < paged_min) {
            uint8_t* ptr = aligned_allocator::allocate(size, alignment);
            return std::vector<std::shared_ptr<Buffer>> {std::make_shared<Buffer> (ptr, size, AllocationType::aligned_alloc, nullptr)};
        }

        if (size < paged_chunked_min){
            uint8_t* ptr = mem::vmem::allocate(size);
            return std::vector {(std::make_shared<Buffer>(ptr, size, AllocationType::page,nullptr))};
        }

        if (size < paged_big_chunked_min) {
            return allocate_chunked(size, 64 * kb);
        }

        return allocate_chunked(size, 1 * mb);
    }


    void deallocate(uint8_t* ptr, const size_t size, const AllocationType type, VmemHandler* vmem_handler) {
        switch (type) {
            case AllocationType::aligned_alloc:
                aligned_allocator::deallocate(ptr);
                return;

            case AllocationType::page:
                vmem::deallocate(ptr, size);
                return;

            case AllocationType::chunked_page:
                vmem::discard_pages(ptr, size);
                if (vmem_handler->subtract_and_test()) {
                    delete vmem_handler;
                }
                return;

            case AllocationType::none:
                throw std::invalid_argument("Invalid allocation type");

            default:
                throw std::runtime_error("Unknown allocation type");
        }

    }



}



