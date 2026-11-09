module;
#include <ostream>
#include <utility>
#include <iostream>

export module atomix.mem.data_storage;

import atomix.config;


import atomix.mem.file_manager;
import atomix.mem.chunk;


export namespace atomix::mem{
    class DataStorage {
        FileManager file_manager_;
        //size_t capacity_; callable with file_manager
        size_t data_size_ = 0;
        PtrReference reference_ = PtrReference::None;
        size_t base_offset_ = 0; //after header usually

    public:
        DataStorage() = default;
        DataStorage(FileManager&& file_manager, const size_t data_size, const PtrReference reference, const size_t base_offset):
        file_manager_(std::move(file_manager)), data_size_(data_size), reference_(reference), base_offset_(base_offset){}


        [[nodiscard]]std::byte* begin()const {
            return file_manager_.begin();
        }

        [[nodiscard]]size_t capacity() const {
            return file_manager_.size();
        }

        [[nodiscard]]size_t data_size() const {
            return data_size_;
        }

        [[nodiscard]]PtrReference get_reference() const {
            return reference_;
        }

        void reserve(const size_t new_size) {
            if (new_size <= capacity()) return;
            file_manager_.resize(new_size);
        }

        void resize(const size_t new_size){
            file_manager_.resize(new_size);
            if (data_size_ > new_size) {
                data_size_ = new_size;
            }
        }

        void safe_resize(const size_t new_size){
            if (data_size_ > new_size) {
                std::cerr << "DataStorage::safe_resize: data_size_ > new_size" << std::endl;
                std::abort();
            }
            file_manager_.resize(new_size);

        }

        [[nodiscard]]Chunk allocate_chunk(const size_t size) {
            
            const size_t real_size =
                (size + default_alignment - 1)
                / default_alignment * default_alignment;

            if (capacity() - data_size_ <  real_size ) {
                reserve(capacity() + real_size);
            }
            const size_t chunk_offset = data_size_;
            data_size_ += real_size;
            return Chunk(chunk_offset, size, reference_);
        }

        [[nodiscard]]DataStorage clone(const std::string& file_name)const{
            DataStorage other;
            other.file_manager_ = file_manager_.clone(file_name);
            other.data_size_ = data_size_;
            other.reference_ = reference_;
            other.base_offset_ = base_offset_;
            return other;
        }






    };
}