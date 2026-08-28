
module;
#include <stdexcept>
#include <sys/mman.h>
#include <cstdint>
#include <fcntl.h>
#include <sys/stat.h>

module atomix.io.reader;

atomix::io::Reader::Reader(const std::string& file_name){

    fd_ = open(file_name.c_str(), O_RDONLY);
    if (fd_ == -1) {
        throw std::runtime_error("Error opening file");
    }

    struct stat s;
    fstat(fd_, &s);

    if (s.st_size < 0) {
        throw std::runtime_error("Error reading file");
    }

    if (s.st_size == 0) {
        throw std::runtime_error("Empty file");
    }

    size_ = s.st_size;

    void* mapped_data = mmap(nullptr, size_, PROT_READ, MAP_PRIVATE, fd_, 0);

    if (mapped_data == MAP_FAILED) {
        throw std::runtime_error("mmap failed");
    }

    begin_ = static_cast<uint8_t*>(mapped_data);

}
