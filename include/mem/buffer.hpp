#ifndef BUFFER_HPP
#define BUFFER_HPP

#include <cstdint>
#include "mem_route.hpp"
#include "allocation_type.hpp"


namespace atomix::mem {
    class Buffer {

    private:
        uint8_t* begin_;
        size_t size_;
        AllocationType alloc_t_;


    public:

        Buffer(uint8_t* begin, const size_t size, const AllocationType alloc_t): begin_(begin), size_(size), alloc_t_(alloc_t) {}
        Buffer(const Buffer&) = delete;
        Buffer& operator=(const Buffer&) = delete;

        Buffer(Buffer&& other)noexcept: begin_(other.begin_), size_(other.size_), alloc_t_(other.alloc_t_) {
            // no need to deallocate yet (using constructor)
            other.alloc_t_ = AllocationType::none;
            other.begin_ = nullptr;
            other.size_ = 0;
        }

        Buffer& operator=(Buffer&& other) noexcept {
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

        ~Buffer(){
            mem_route::deallocate(begin_, alloc_t_);
        }

        [[nodiscard]] uint8_t* get_begin() const{
            return begin_;
        }

        [[nodiscard]] size_t get_size() const {
            return size_;
        }

        [[nodiscard]] AllocationType get_alloc_t() const {
            return alloc_t_;
        }



    };
}












#endif

