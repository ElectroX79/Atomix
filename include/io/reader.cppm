module;
#include <stdexcept>
#include <sys/mman.h>
#include <cstdint>

#include <unistd.h>


export module atomix.io.reader;

export namespace atomix::io {
    class Reader {
    private:
        int fd_ = -1;
        size_t size_;
        uint8_t* begin_ = nullptr;



    public:

        Reader(const Reader&) = delete;
        Reader& operator=(const Reader&) = delete;
        Reader(Reader&& other)noexcept: fd_(other.fd_), size_(other.size_), begin_(other.begin_) {}
        Reader& operator=(Reader&& other)noexcept{
            if (this != &other) {
                munmap(static_cast<void*>(begin_), size_);
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


        /**
         * @brief the constructor using the file_name with the calls open(), fstat()
         * and mmap() gets the file_descriptor (fd_), the size of the file (size_),
         * and a pointer which points on the first page of the file.
         * @param file_name
         */
        explicit Reader(const std::string& file_name);


        /**
         * @brief the destructor asserts that the class terminates safely,
         * closing the temporal token (file descriptor) and liberating the mapped pages.
         */
        ~Reader(){
            munmap(static_cast<void*>(begin_), size_);
            close(fd_);
        }

        int fd() const {
            return fd_;
        }

        size_t size() const {
            return size_;
        }

        const uint8_t* begin() const {
            return begin_;
        }






    };
}









