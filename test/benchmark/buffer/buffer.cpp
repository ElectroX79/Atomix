#include <memory>
#include "../external/catch2/catch_amalgamated.hpp"

import atomix.mem;
import atomix.config;

namespace {
    std::vector<atomix::mem::Buffer> create_buffers(const size_t byte_size){
        std::vector<atomix::mem::Buffer> buffers1;
        buffers1.reserve((byte_size / (atomix::mem::chunk_size)) + 1);

        const size_t remainder = (byte_size % (atomix::mem::chunk_size));

        for (size_t i = 0; i < byte_size / (atomix::mem::chunk_size); ++i) {
            buffers1.push_back(atomix::mem::Buffer(atomix::mem::chunk_size, atomix::mem::default_alignment));
        }
        if (remainder != 0){
            buffers1.push_back(atomix::mem::Buffer(remainder, atomix::mem::default_alignment));
        }
        return buffers1;
    }
}

//Note: BENCHMARK in Catch2 repeats the same operation 100 times

TEST_CASE("Creation and destruction", "[buffer]") {

    BENCHMARK("Creation and destruction (RAII) ") {
        constexpr size_t repeat = 1000;
        for (size_t i = 0; i < repeat; i++) {
            const std::vector<atomix::mem::Buffer> buffers =  create_buffers(atomix::units::KiB);
        }
    };

}


TEST_CASE("Copying", "[buffer]") {
    size_t chunk_size;

    const std::vector<atomix::mem::Buffer> buffers = create_buffers(atomix::units::MiB);
    std::vector<atomix::mem::Buffer> buffers_dest;
    buffers_dest.reserve(buffers.size());

    for (auto& buffer : buffers) {
        memset(buffer.get_begin(), 0xAB, buffer.get_size());
    }

    BENCHMARK("Copy") {
        constexpr size_t repeat = 1000;
        for (size_t i = 0; i < repeat; i++) {
            buffers_dest = buffers;
        }
    };

}


TEST_CASE("Moving", "[buffer]") {
    std::vector<atomix::mem::Buffer> buffers = create_buffers(atomix::units::MiB);
    std::vector<atomix::mem::Buffer> buffers_dest;
    buffers_dest.reserve(buffers.size());

    for (auto& buffer : buffers) {
        memset(buffer.get_begin(), 0xAB, buffer.get_size());
    }

    BENCHMARK("Moving") {
        constexpr size_t repeat = 1000;
        for (size_t i = 0; i < repeat; i++) {
            buffers_dest = std::move(buffers);
            buffers = std::move(buffers_dest);
        }
        // Reversing change because CATCH2 repeats the operation x times
    };

}





TEST_CASE("Accessing", "[buffer]") {
    std::vector<atomix::mem::Buffer> buffers = create_buffers(atomix::units::MiB);

    for (auto& buffer : buffers) {
        memset(buffer.get_begin(), 0xAB, buffer.get_size());
    }

    uint32_t acc = 0;
    BENCHMARK("Accessing") {
        constexpr size_t repeat = 1000;
        for (size_t i = 0; i < repeat; i++) {
            for (auto& buffer : buffers) {
                for (size_t i = 0; i < buffer.get_size(); i+=4) {
                    uint32_t temp;
                    std::memcpy(&temp, buffer.get_begin() + i,  sizeof(uint32_t));
                    acc += temp;
                }
            }
        }
    };

}

