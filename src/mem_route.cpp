module;
#include <cstring>
#include <vector>
#include <memory>
#include <iostream>
#include "cassert"

module atomix.mem.mem_route;

import atomix.mem.buffer;
import atomix.mem.allocation_type;
import atomix.mem.aligned_allocator;
import atomix.bounds;

namespace atomix::mem::mem_route{
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
    constexpr uint64_t kb = 1024;
    constexpr uint64_t paged_min = 64 * kb;
    constexpr size_t one_chunk_size = 64 * kb;

     [[nodiscard]] std::vector<Buffer> allocate(const size_t size, size_t& chunk_size, const size_t alignment ) {


        if (size < paged_min) {
            chunk_size = 0;
            std::vector<Buffer> v;
            v.push_back(allocate_buffer(size, AllocationType::aligned_alloc, alignment));
            return v;
        }

        chunk_size = one_chunk_size;
        return allocate_chunked(size, chunk_size, alignment);
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

    [[nodiscard]]Buffer allocate_buffer(const size_t size, const AllocationType alloc_t, const size_t alignment) {
         switch (alloc_t) {

             case AllocationType::aligned_alloc:
                 return Buffer(aligned_allocator::allocate(size, alignment), size, alloc_t);

             case AllocationType::none:
                 std::cerr << "Precondition broken: { alloc_t == AllocationType::none }, on atomix::mem::mem::route::allocate_buffer \n";
                 std::abort();
         }

     }

    /*
     [[nodiscard]] std::vector<std::shared_ptr<Buffer>> copy(const std::vector<std::shared_ptr<Buffer>>& ref, const std::vector<CopyType>& copy_types) {
        if (copy_types.size() != ref.size()) {
            std::cerr << "Precondition broken: { copy_types.size() == ref.size() }, on atomix::mem::mem::route::copy_deep \n";
            std::abort();
        }
        std::vector<std::shared_ptr<Buffer>> aux;
        aux.reserve(ref.size());


        if (ref[0]->get_alloc_t() == AllocationType::none) {
            //needs another strategy, for now that doesn't matter (all uses aligned allocation), but in the future,
            //that could be of some import.
        }



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

    */

}



