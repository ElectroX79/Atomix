module;
#include <cstring>
#include <cstddef>

module atomix.mem.buffer;
import atomix.mem.mem_route;
import atomix.config;

namespace atomix::mem {

    Buffer::Buffer(size_t size, size_t alignment = default_alignment) {
        const auto info = mem_route::allocate(size, alignment);
        begin_ = info.ptr;
        size_ = info.size;
        alloc_t_ = info.alloc_t;
    }

    //Destructor
    Buffer::~Buffer() {
        mem_route::deallocate(begin_, alloc_t_);
    }


    //Copy
    Buffer::Buffer(const Buffer& other)noexcept{
        const auto info = mem_route::allocate(other.size_, default_alignment);
        begin_ = info.ptr;
        size_ = info.size;
        alloc_t_ = info.alloc_t;
        memcpy(begin_, other.begin_, other.size_);
    }

    Buffer& Buffer::operator=(const Buffer& other) noexcept {
        if (this != &other) {
            mem_route::deallocate(begin_, alloc_t_);
            const auto info = mem_route::allocate(other.size_, default_alignment);
            begin_ = info.ptr;
            size_ = info.size;
            alloc_t_ = info.alloc_t;
            memcpy(begin_, other.begin_, other.size_);
        }
        return *this;
    }


    //Move
    Buffer::Buffer(Buffer&& other)noexcept: begin_(other.begin_), size_(other.size_), alloc_t_(other.alloc_t_) {
        // no need to deallocate yet (using constructor)
        other.alloc_t_ = AllocationType::none;
        other.begin_ = nullptr;
        other.size_ = 0;
    }

    Buffer& Buffer::operator=(Buffer&& other) noexcept {
        if (this != &other) {
            mem_route::deallocate(begin_, alloc_t_);

            begin_ = other.begin_;
            size_ = other.size_;
            alloc_t_ = other.alloc_t_;

            other.begin_ = nullptr;
            other.size_ = 0;
            other.alloc_t_ = AllocationType::none;
        }
        return *this;

    }
}

