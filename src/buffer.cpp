
#include "../include/mem/buffer.hpp"

#include <cstring>

#include "../include/mem/mem_route.hpp"


namespace atomix::mem {

    //Destructor
    Buffer::~Buffer() {
        mem_route::deallocate(begin_, alloc_t_);
    }


    //Copy
    Buffer::Buffer(const Buffer& other)noexcept:
        Buffer(mem_route::allocate_buffer(other.size_, other.alloc_t_)){
        memcpy(begin_, other.begin_, other.size_);
    }
    Buffer& Buffer::operator=(const Buffer& other) noexcept {
        *this = mem_route::allocate_buffer(other.size_, other.alloc_t_);
        memcpy(begin_, other.begin_, other.size_);
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
