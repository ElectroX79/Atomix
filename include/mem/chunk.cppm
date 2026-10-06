module;

#include <cstddef>
#include <cstdint>



export module atomix.mem.chunk;
import atomix.bounds;


export namespace atomix::mem {

    enum class PtrReference: uint8_t {
        None,
        Main,
        Variable
    };
    struct Chunk {
        uint64_t offset_ = 0;
        uint64_t size_ = 0;
        PtrReference reference_ = PtrReference::None;

        Chunk() = default;

        Chunk(const uint64_t offset,const uint64_t size, const PtrReference reference) :
        offset_(offset), size_(size), reference_(reference)  {}


        ~Chunk() = default;
        Chunk(const Chunk&) = delete;
        Chunk& operator=(const Chunk&) = delete;

        Chunk(Chunk&& other) noexcept:
        offset_(other.offset_),
        size_(other.size_),
        reference_(other.reference_){
            other.offset_ = 0;
            other.size_ = 0;
            other.reference_ = PtrReference::None;
        }

        Chunk& operator=(Chunk&& other) noexcept{
            if (&other != this) {
                offset_ = other.offset_;
                size_ = other.size_;
                reference_ = other.reference_;

                other.offset_ = 0;
                other.size_ = 0;
                other.reference_ = PtrReference::None;
            }
            return *this;
        }

        //WARNING: do not use if you are not completely sure of the consequences
        [[nodiscard]]Chunk unsafe_clone() const {
            return Chunk(offset_, size_, reference_);
        }

        bool operator==(const Chunk &) const = default;
        bool operator!=(const Chunk &) const = default;

        [[nodiscard]] std::byte* calculate_ptr(const size_t offset, std::byte* ptr, size_t type_size) const {
            const size_t rel_offset = offset * type_size;
            const size_t new_offset = offset_ + rel_offset;
            bounds::check_offset_length(rel_offset, type_size, size_);
            return ptr + new_offset;
        }
    };


}