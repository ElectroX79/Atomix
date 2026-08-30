module;

#include <cstddef>


export module atomix.config;

export namespace atomix {
    namespace units {
        inline constexpr size_t KiB = 1024;
        inline constexpr size_t MiB = 1024*KiB;
        inline constexpr size_t GiB = 1024*MiB;
        inline constexpr size_t TiB = 1024*GiB;
        inline constexpr size_t PiB = 1024*TiB;

        inline constexpr size_t kilo = 1000;
        inline constexpr size_t mega = 1000*kilo;
        inline constexpr size_t giga = 1000*mega;
        inline constexpr size_t tera = 1000*giga;
        inline constexpr size_t peta = 1000*tera;
    }

    namespace mem {
        inline constexpr size_t chunk_size = 128 * units::KiB;
        inline constexpr size_t cut_size = 64 * units::KiB;

        inline constexpr size_t default_alignment = 64;
    }
}