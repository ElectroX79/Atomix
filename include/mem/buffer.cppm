module;
#include <cstdint>
#include <cstddef>

export module atomix.mem.buffer;
import atomix.mem.allocation_type;

export namespace atomix::mem {
    class Buffer {

    private:
        uint8_t* begin_ = nullptr;
        size_t size_ = 0;
        AllocationType alloc_t_ = AllocationType::none;


    public:

        Buffer(uint8_t* begin, const size_t size, const AllocationType alloc_t): begin_(begin), size_(size), alloc_t_(alloc_t) {}

        Buffer(size_t size, size_t alignment);

        Buffer(const Buffer& other) noexcept;
        Buffer& operator=(const Buffer& other) noexcept;
        Buffer(Buffer&& other)noexcept;
        Buffer& operator=(Buffer&& other) noexcept;
        ~Buffer();



        [[nodiscard]] uint8_t* get_begin() const{
            return begin_;
        }

        [[nodiscard]] size_t get_size() const {
            return size_;
        }

        [[nodiscard]] AllocationType get_alloc_t() const {
            return alloc_t_;
        }

        [[nodiscard]] bool non_owner() const {
            return alloc_t_ == AllocationType::none;
        }




    };
}













