#ifndef BUFFER_HPP
#define BUFFER_HPP

#include <cstdint>
#include <stdexcept>
#include "mem_route.hpp"

#include "allocation_type.hpp"


namespace atomix::mem {
    class Buffer {

    private:
        uint8_t* begin_;
        size_t size_;
        AllocationType type_;
        VmemHandler* vmem_handler_;

    public:

        Buffer(uint8_t* begin, const size_t size, const AllocationType type, VmemHandler* vmem_handler): begin_(begin), size_(size), type_(type), vmem_handler_(vmem_handler){}
        Buffer(const Buffer&) = delete;
        Buffer& operator=(const Buffer&) = delete;

        Buffer(Buffer&& other)noexcept: begin_(other.begin_), size_(other.size_), type_(other.type_), vmem_handler_(other.vmem_handler_){
            // no need to deallocate yet (using constructor)
            other.begin_ = nullptr;
            other.size_ = 0;
            other.type_ = AllocationType::none;
            other.vmem_handler_ = nullptr;
        }

        Buffer& operator=(Buffer&& other) noexcept {
            if (this != &other) {
                mem_route::deallocate(begin_, size_, type_, vmem_handler_);

                begin_ = other.begin_;
                size_ = other.size_;
                type_ = other.type_;
                vmem_handler_ = other.vmem_handler_;

                other.begin_ = nullptr;
                other.size_ = 0;
                other.type_ = AllocationType::none;
                other.vmem_handler_ = nullptr;
            }
            return *this;

        }

        ~Buffer(){
            mem_route::deallocate(begin_, size_, type_, vmem_handler_);
        }

        [[nodiscard]] uint8_t* get_begin() const{
            return begin_;
        }

        [[nodiscard]] size_t get_size() const {
            return size_;
        }

        [[nodiscard]] AllocationType alloc_type()const {
            return type_;
        }

    };
}












#endif

