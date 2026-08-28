module;
#include <vector>
#include <memory>
#include <cstdint>

export module atomix.mem.mem_route;
import atomix.mem.buffer;
import atomix.mem.allocation_type;



export namespace atomix::mem{
    /*
    enum class CopyType: uint8_t {
        Deep,
        Shallow,
        Reinitialize
    };
    */


    namespace mem_route {
        // Future change: custom allocator, see documentation

        [[nodiscard]] std::vector<Buffer> allocate( size_t size, size_t& chunk_size,size_t alignment = 64);
        void deallocate(uint8_t* ptr, AllocationType alloc_t);

        Buffer allocate_buffer(size_t size, AllocationType alloc_t, size_t alignment = 64);

       // [[nodiscard]] std::vector<std::shared_ptr<Buffer>> copy( const std::vector<std::shared_ptr<Buffer>>& ref, const std::vector<CopyType>& copy_types);

    }

}




