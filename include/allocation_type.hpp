#ifndef ALLOCATION_TYPE
#define ALLOCATION_TYPE

#include <cstdint>
#include <atomic>
#include <span>

#include "vmem.hpp"

namespace atomix::mem {

    enum class AllocationType : uint8_t {
        aligned_alloc,
        page, //4KB
        chunked_page,
        none,
    };

    class VmemHandler{
        std::span<uint8_t> data_;
        std::atomic<uint64_t> counter_;

    public:
        VmemHandler(uint8_t* ptr, const size_t size, const uint64_t counter ) : data_(ptr, size), counter_(counter) {}
        VmemHandler(const std::span<uint8_t> data, const uint64_t counter ) : data_(data), counter_(counter) {}

        VmemHandler(const VmemHandler&) = delete;
        VmemHandler& operator=(const VmemHandler&) = delete;
        VmemHandler(VmemHandler&& other) = delete;
        VmemHandler& operator=(VmemHandler&& other) = delete;

        ~VmemHandler(){
            vmem::deallocate(data_.data(), data_.size());
        }

        bool subtract_and_test() {
            --counter_;
            return counter_ == 0;
        }
    };
}



#endif
