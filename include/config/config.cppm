module;

#include <cstddef>
#include <fstream>
#include <iostream>
#include <string>
#include <cstdlib>


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


    namespace machine {
        std::size_t total_memory_linux() {
            std::ifstream file("/proc/meminfo");

            if (!file.is_open()) {
                std::cerr << "Could not open /proc/meminfo\n";
                std::abort();
            }

            std::string key;
            std::size_t value;
            std::string unit;

            while (file >> key >> value >> unit) {
                if (key == "MemTotal:") {
                    if (unit == "kB") {
                        return value * units::KiB;
                    }

                    std::cerr << "Unsupported unit: " << unit << '\n';
                    std::abort();
                }
            }
            file.close();

            std::cerr << "Cannot find MemTotal in /proc/meminfo\n";
            std::abort();
        }
        const size_t total_memory = total_memory_linux();
    }

    namespace mem {
        inline constexpr size_t chunk_size = 128 * units::KiB;
        inline constexpr size_t cut_size = 64 * units::KiB;

        inline constexpr size_t default_alignment = 64;

        namespace arena_allocator {

            inline constexpr size_t chunk_size = 16 * units::KiB;
            inline constexpr size_t arena_size = 16 * units::MiB;

            const size_t max_size = machine::total_memory;
            namespace {
                constexpr size_t expected_initial_size = 16 * units::MiB;
                size_t calculate_initial_size(const size_t expected_size) {
                    if (max_size < expected_size / 2 ) {
                        return max_size / 2;
                    }
                    return expected_initial_size;
                }
            }
            const size_t initial_size = calculate_initial_size(expected_initial_size); // Do NOT change here directly, change expected_initial_size

        }
    }


}