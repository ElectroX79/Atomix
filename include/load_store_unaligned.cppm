module;
#include <cstddef>
#include <cstring>
#include <type_traits>
export module atomix.load_unaligned;

export namespace atomix {
    template <typename T>
    concept TriviallyCopyable = std::is_trivially_copyable_v<T>;


    /**
      * @brief Safely reads a trivially copyable value from an unaligned byte buffer.
      *
      * Uses std::memcpy to bypass strict aliasing and alignment UB when deserializing
      * packed structures or arbitrary byte offsets.
      *
      * @note If the source buffer is already properly aligned (alignof(T)), you should prefer
      *          std::start_lifetime_as to achieve zero-copy semantics without stack copies.
      */
    template <TriviallyCopyable T>
    [[nodiscard]] T load_unaligned(const std::byte* src)noexcept {
        T val;
        std::memcpy(&val, src, sizeof(T));
        return val;
    }

    template <TriviallyCopyable T>
    void store_unaligned(std::byte* dest, const T& val)noexcept {
        std::memcpy(dest, &val , sizeof(T));
    }




}