module;
#include <sys/mman.h>
#include <cstdint>
#include <stdexcept>
#include <iostream>
#include <ostream>

export module atomix.mem.vmem;


export namespace atomix::mem::vmem{
    [[nodiscard]] inline uint8_t* allocate (const size_t size) {
        const auto ptr = static_cast<uint8_t *> (mmap(nullptr, size,PROT_READ | PROT_WRITE,MAP_PRIVATE | MAP_ANONYMOUS , -1, 0));
        if (ptr == MAP_FAILED) {
            throw std::bad_alloc();
        }

        return ptr;
    }

    inline void deallocate(uint8_t* ptr, const size_t size) {
        if (ptr!= MAP_FAILED && ptr != nullptr){
            munmap(ptr, size);
        }
    }

    inline void discard_pages(uint8_t* ptr, const size_t size) {
        const int rt = madvise(ptr, size, MADV_DONTNEED);

        if (rt != 0) {
            const auto error = errno;
            std::cerr <<"madvise failed"<<  error << std::endl;

        }

    }
};



