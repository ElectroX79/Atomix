module;
#include <cstdint>
#include <stdexcept>
#include <system_error>

export module atomix.mem.aligned_allocator;


export namespace atomix::mem::aligned_allocator{

    [[nodiscard]]inline uint8_t* allocate(const size_t size, const size_t alignment = 64) {

        const size_t real_size = ((size + alignment - 1) / alignment) * alignment;

        const auto ptr = static_cast<uint8_t *>(aligned_alloc(alignment, real_size));

        if (ptr == nullptr) {
            if (errno == ENOMEM) {
                const int error = errno;
                throw std::system_error(error, std::system_category(), "Out of memory");
            }
            if (errno == EINVAL) {
                throw std::invalid_argument("The alignment argument was not a power of two.");
            }
            throw std::bad_alloc();
        }
        return ptr;
    }

    inline void deallocate (uint8_t* ptr) {
        if (ptr == nullptr) {
            throw std::invalid_argument("The pointer cannot be nullptr");
        }
        free(reinterpret_cast<void*>(ptr));
    }





}





