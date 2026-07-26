#ifndef VMEM_HPP
#define VMEM_HPP

#include <sys/mman.h>
#include <cstdint>
#include <stdexcept>

namespace atomix::vmem{
    [[nodiscard]] inline uint8_t* allocate (const size_t size) {
        const auto data = static_cast<uint8_t *> (mmap(nullptr, size,PROT_READ | PROT_WRITE,MAP_PRIVATE | MAP_ANONYMOUS , -1, 0));
        if (data == MAP_FAILED) {
            throw std::bad_alloc();
        }

        return data;
    }

    inline void deallocate(uint8_t* data, const size_t size) {
        if (data!= MAP_FAILED && data != nullptr){
            munmap(data, size);
        }
    }
};










#endif
