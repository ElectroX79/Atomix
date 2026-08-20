#include <cstring>
#include <vector>
#include <memory>
#include <iostream>

#include "cassert"
#include "mem/aligned_allocator.hpp"
#include "mem/buffer.hpp"
#include "mem/allocation_type.hpp"
#include "../include/security_check.hpp"

namespace atomix::mem::mem_route{
    namespace {
         [[nodiscard]] std::vector<std::shared_ptr<Buffer>> allocate_chunked(const size_t size, const size_t chunk_size) {
            assert(size >= chunk_size);

            std::vector<std::shared_ptr<Buffer>> v;
            v.reserve((size / (chunk_size)) + 1);

            const size_t remainder = (size % (chunk_size));

            for (size_t i = 0; i < size / (chunk_size); ++i) {
                uint8_t* ptr = aligned_allocator::allocate(chunk_size);
                v.push_back(std::make_shared<Buffer>(ptr, chunk_size, AllocationType::aligned_alloc));
            }
            if (remainder != 0){
                uint8_t* ptr = aligned_allocator::allocate(remainder);
                v.push_back(std::make_shared<Buffer>(ptr, remainder, AllocationType::aligned_alloc));
            }
            return v;
        }
    }
    constexpr uint64_t kb = 1024;
    constexpr uint64_t paged_min = 64 * kb;
    constexpr size_t one_chunk_size = 64 * kb;

     [[nodiscard]] std::vector<std::shared_ptr<Buffer>> allocate(const size_t size, size_t& chunk_size, const size_t alignment) {


        if (size < paged_min) {
            uint8_t* ptr = aligned_allocator::allocate(size, alignment);
            chunk_size = 0;
            return std::vector<std::shared_ptr<Buffer>>{std::make_shared<Buffer> (ptr, size, AllocationType::aligned_alloc)};
        }

        chunk_size = one_chunk_size;
        return allocate_chunked(size, chunk_size);
    }


    void deallocate(uint8_t* ptr, const AllocationType alloc_t) {
        switch (alloc_t) {

            case AllocationType::aligned_alloc:
                aligned_allocator::deallocate(ptr);
                return;

            case AllocationType::none:
                throw std::runtime_error("Allocation type is none");

        }
    }

     [[nodiscard]] std::vector<std::shared_ptr<Buffer>> copy(const std::vector<std::shared_ptr<Buffer>>& ref, const std::vector<CopyType>& copy_types) {
        if (copy_types.size() != ref.size()) {
            std::cerr << "Precondition broken: { copy_types.size() == ref.size() }, on atomix::mem::mem::route::copy_deep \n";
            std::abort();
        }
        std::vector<std::shared_ptr<Buffer>> aux;
        aux.reserve(ref.size());

        /*
        if (ref[0]->get_alloc_t() == AllocationType::none) {
            //needs another strategy, for now that doesn't matter (all uses aligned allocation), but in the future,
            //that could be of some import.
        }
        */


        for (size_t i = 0; i < copy_types.size(); ++i) {
            switch (copy_types[i]) {
                case CopyType::Shallow:
                    aux.push_back(ref[i]);
                    break;
                case CopyType::Deep:
                    aux.push_back(std::make_shared<Buffer>( aligned_allocator::allocate(ref[i]->get_size()),
                                                            ref[i]->get_size(),
                                                            ref[i]->get_alloc_t()
                                                            ));
                    memcpy(aux.back()->get_begin(), ref[i]->get_begin(), ref[i]->get_size());
                    break;
                case CopyType::Reinitialize:
                   aux.push_back(std::make_shared<Buffer>( aligned_allocator::allocate(ref[i]->get_size()),
                                                            ref[i]->get_size(),
                                                            ref[i]->get_alloc_t()
                                                            ));
                    break;
            }
        }
        return aux;
    }



}



