#ifndef BUFFER_HPP
#define BUFFER_HPP

#include <cstdint>
#include "mem_route.hpp"


namespace atomix::mem {
    class Buffer {

    private:
        uint8_t* begin_;
        size_t size_;

    public:

        Buffer(uint8_t* begin, const size_t size): begin_(begin), size_(size){}
        Buffer(const Buffer&) = delete;
        Buffer& operator=(const Buffer&) = delete;

        Buffer(Buffer&& other)noexcept: begin_(other.begin_), size_(other.size_){
            // no need to deallocate yet (using constructor)
            other.begin_ = nullptr;
            other.size_ = 0;
        }

        Buffer& operator=(Buffer&& other) noexcept {
            if (this != &other) {
                mem_route::deallocate(begin_);

                begin_ = other.begin_;
                size_ = other.size_;

                other.begin_ = nullptr;
                other.size_ = 0;
            }
            return *this;

        }

        ~Buffer(){
            mem_route::deallocate(begin_);
        }

        [[nodiscard]] uint8_t* get_begin() const{
            return begin_;
        }

        [[nodiscard]] size_t size() const {
            return size_;
        }

    };
}












#endif

