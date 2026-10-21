module;

#include <cstddef>


export module atomix.config;

export namespace atomix {

    constexpr bool is_power_two(const size_t n) {
        //n > 0 redundant,
        return  n > 0 && (n & (n - 1)) == 0;
    }

    inline constexpr size_t KiB = 1ULL << 10;
    inline constexpr size_t MiB = 1ULL << 20;
    inline constexpr size_t GiB = 1ULL << 30;
    inline constexpr size_t TiB = 1ULL << 40;
    inline constexpr size_t PiB = 1ULL << 50;

    inline constexpr size_t kilo = 1000ULL;
    inline constexpr size_t mega = 1000ULL*kilo;
    inline constexpr size_t giga = 1000ULL*mega;
    inline constexpr size_t tera = 1000ULL*giga;
    inline constexpr size_t peta = 1000ULL*tera;

    inline constexpr size_t n_bits_byte = 8;

    namespace mem {
        inline constexpr size_t chunk_size = 128 * KiB;
        inline constexpr size_t cut_size = 64 * KiB;

        inline constexpr size_t default_alignment = 64;

        inline constexpr size_t atomix_format_version = 1;
        inline constexpr size_t atomix_format_main_header_size = 64;
        inline constexpr size_t atomix_format_var_header_size = 64;
    }
    namespace data {
        inline constexpr size_t n_rows_batch = 1024;
    }

    static_assert(mem::default_alignment > 0 &&
        "default_alignment must be positive");

    static_assert(is_power_two(mem::default_alignment) &&
        "default_alignment must be power of two");

    //

    static_assert(mem::atomix_format_main_header_size > 0 &&
        "atomix_format_main_header_size must be positive");

    static_assert(is_power_two(mem::atomix_format_main_header_size) &&
        "atomix_format_main_header_size must be power of two");

    static_assert(mem::atomix_format_main_header_size % mem::default_alignment == 0 &&
       "atomix_format_main_header_size must be divisible by default_alignment");

    //

    static_assert(mem::atomix_format_var_header_size > 0 &&
        "atomix_format_var_header_size must be positive");

    static_assert(is_power_two(mem::atomix_format_var_header_size) &&
        "atomix_format_var_header_size must be power of two");

    static_assert(mem::atomix_format_var_header_size % mem::default_alignment == 0 &&
       "atomix_format_var_header_size must be divisible by default_alignment");

    //

    static_assert(mem::cut_size > 0 &&
        "Cut size must be positive");

    static_assert(mem::chunk_size > 0 &&
        "Chunk size must be positive");

    //

    static_assert(data::n_rows_batch > 0 &&
        "n_row_batch must be positive");

    static_assert(is_power_two(data::n_rows_batch) &&
        "n_row_batch must be power of two");

    static_assert(data::n_rows_batch % mem::default_alignment == 0 &&
        "n_row_batch must be divisible by default_alignment");



}




