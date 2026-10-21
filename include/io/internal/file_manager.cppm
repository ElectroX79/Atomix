module;

#include <cstddef>
#include <unistd.h>
#include <sys/mman.h>
#include <stdexcept>
#include <string>
#include <fstream>
#include <fcntl.h>
#include <sys/stat.h>




export module atomix.mem.file_manager;

import atomix.data.data_type;
import atomix.mem.chunk;



export namespace atomix::mem {

    class FileManager {
        int fd_ = -1;
        std::byte* begin_ = nullptr;
        size_t size_ = 0;

    public:
        FileManager() = default;
        FileManager(const FileManager&) = delete;
        FileManager& operator=(const FileManager&) = delete;
        FileManager(FileManager&& other)noexcept:
        fd_(other.fd_),
        begin_(other.begin_),
        size_(other.size_)
        {
            other.fd_ = -1;
            other.begin_ = nullptr;
            other.size_ = 0;
        }
        FileManager& operator=(FileManager&& other)noexcept{
            if (this != &other) {

                if (begin_ != nullptr)
                    munmap(static_cast<void*>(begin_), size_);

                if (fd_ != -1)
                    close(fd_);

                this->fd_ = other.fd_;
                this->size_ = other.size_;
                this->begin_ = other.begin_;

                other.fd_ = -1;
                other.size_ = 0;
                other.begin_ = nullptr;
            }
            return *this;
        }

        explicit FileManager(const std::string& file_name) {
            reinitialize(file_name);
        }

        ~FileManager() {
            if (begin_ != nullptr)
                munmap(static_cast<void*>(begin_), size_);

            if (fd_ != -1)
                close(fd_);
        }

        void reinitialize(const std::string& file_name) {

            if (begin_ != nullptr)
                munmap(static_cast<void*>(begin_), size_);

            if (fd_ != -1)
                close(fd_);

            begin_ = nullptr;
            fd_ = -1;
            size_ = 0;


            fd_ = open(file_name.c_str(), O_RDWR);
            if (fd_ == -1) {
                throw std::runtime_error("Error opening file");
            }

            struct stat s = {};
            fstat(fd_, &s);

            if (s.st_size < 0) {
                throw std::runtime_error("Error reading file");
            }

            if (s.st_size == 0) {
                throw std::runtime_error("Empty file");
            }

            size_ = s.st_size;

            void* mapped_data = mmap(nullptr, size_, PROT_READ | PROT_WRITE, MAP_SHARED, fd_, 0);

            if (mapped_data == MAP_FAILED) {
                throw std::runtime_error("mmap failed");
            }

            begin_ = static_cast<std::byte*>(mapped_data);
        }

        [[nodiscard]]std::byte* begin() const {
            return begin_;
        }
        [[nodiscard]]size_t size() const {
            return size_;
        }

        void resize(const size_t new_size){
            if (new_size == size_) return;

            if (ftruncate(fd_, static_cast<off_t>(new_size)) == -1) {
                throw std::runtime_error("Error expanding file, ftruncate failed");
            }

            void* ptr = mremap(begin_, size_, new_size, MREMAP_MAYMOVE);
            //Notice mremap is linux-only function; we may consider other options
            if (ptr == MAP_FAILED) {
                throw std::runtime_error("Error expanding file, mremap failed");
            }

            begin_ = static_cast<std::byte*>(ptr);
            size_ = new_size;
        }

        [[nodiscard]]FileManager clone(const std::string& file_name)const{

            FileManager other;

            other.fd_ = open(file_name.c_str(), O_RDWR | O_CREAT | O_EXCL, 0644);

            if (other.fd_ == -1) {
                throw std::runtime_error("Error opening file");
            }

            if (ftruncate(other.fd_, static_cast<off_t> (size_)) == -1) {
                throw std::runtime_error("Error expanding file, ftruncate failed");
            }

            loff_t byte_to_copy = size_;

            while (byte_to_copy > 0) {
                const ssize_t bytes_copied = copy_file_range(fd_, nullptr, other.fd_, nullptr, byte_to_copy, 0);
                if (bytes_copied == -1) {
                    throw std::runtime_error("Error writing file");
                }

                if (bytes_copied == 0) {
                    throw std
                    ::runtime_error("Unexpected EOF while copying file");
                }

                byte_to_copy -= bytes_copied;
            }
            other.size_ = size_;
            void* mapped_data = mmap(nullptr, other.size_, PROT_READ | PROT_WRITE, MAP_SHARED, other.fd_, 0);

            if (mapped_data == MAP_FAILED) {
                throw std::runtime_error("mmap failed");
            }

            other.begin_ = static_cast<std::byte*>(mapped_data);

            return other;
        }



        [[nodiscard]] static FileManager create_blank(const std::string& file_name, const size_t size){

            if (size == 0) {
                throw std::invalid_argument("Size cannot be 0");
            }
            FileManager other;

            other.fd_ = open(file_name.c_str(), O_RDWR | O_CREAT | O_TRUNC, 0644);

            if (other.fd_ == -1) {
                throw std::runtime_error("Error opening file");
            }

            if (ftruncate(other.fd_, static_cast<off_t> (size))) {
                throw std::runtime_error("Error expanding file, ftruncate failed");
            }

            other.size_ = size;
            void* mapped_data = mmap(nullptr, other.size_, PROT_READ | PROT_WRITE, MAP_SHARED, other.fd_, 0);

            if (mapped_data == MAP_FAILED) {
                throw std::runtime_error("mmap failed");
            }

            other.begin_ = static_cast<std::byte*>(mapped_data);

            return other;
        }





    };
}