module;
#include <vector>
#include <memory>
#include <cstdint>

export module atomix.mem.mem_route;
import atomix.mem.buffer;
import atomix.mem.allocation_type;
import atomix.config;



export namespace atomix::mem{
     struct AllocInfo {
        uint8_t* ptr;
        size_t size;
        AllocationType alloc_t;
    };

    /*
    enum class CopyType: uint8_t {
        Deep,
        Shallow,
        Reinitialize
    };
    */


    namespace mem_route {
        // Future change: custom allocator, see documentation

        [[nodiscard]] AllocInfo allocate( size_t size, size_t alignment = default_alignment);
        void deallocate(uint8_t* ptr, AllocationType alloc_t);

    }

}




