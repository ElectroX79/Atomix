module;
#include <cstdint>
#include <stdexcept>
#include <system_error>
#include <cstddef>

export module atomix.mem.aligned_allocator;


export namespace atomix::mem::aligned_allocator{

    [[nodiscard]]inline std::byte* allocate(const size_t size, const size_t alignment = 64) {

        const size_t real_size = ((size + alignment - 1) / alignment) * alignment;

        const auto ptr = static_cast<std::byte *>(aligned_alloc(alignment, real_size));

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

    inline void deallocate (std::byte* ptr)noexcept {
        free(reinterpret_cast<void*>(ptr));
    }





}





