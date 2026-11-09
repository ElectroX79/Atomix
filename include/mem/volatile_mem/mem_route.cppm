module;
#include <vector>
#include <memory>
#include <cstdint>

export module atomix.mem.mem_route;
import atomix.mem.allocation_type;
import atomix.config;



export namespace atomix::mem{
     struct AllocInfo {
        std::byte* ptr;
        size_t size;
        AllocationType alloc_t;
    };

    /*
    enum class CopyType: std::byte {
        Deep,
        Shallow,
        Reinitialize
    };
    */


    namespace mem_route {
        // Future change: custom allocator, see documentation

        [[nodiscard]] AllocInfo allocate( size_t size, size_t alignment = default_alignment);
        void deallocate(std::byte* ptr, AllocationType alloc_t);

    }

}




