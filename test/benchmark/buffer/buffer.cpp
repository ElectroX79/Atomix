#include <memory>
#include "../external/catch2/catch_amalgamated.hpp"
#include "../../../include/mem/mem_route.hpp"
#include "../../../include/mem/buffer.hpp"



//Note: BENCHMARK in Catch2 repeats the same operation 100 times

TEST_CASE("Creation and destruction", "[buffer]") {

    BENCHMARK("Creation and destruction (RAII) ") {
        constexpr size_t repeat = 1000;
        for (size_t i = 0; i < repeat; i++) {
            size_t chunk_size;
            const std::vector<std::shared_ptr<atomix::mem::Buffer>> buffers = atomix::mem::mem_route::allocate(1024, chunk_size, 64);
        }
    };

}

TEST_CASE("Shallow Copying", "[buffer]") {
    size_t chunk_size;
    const std::vector<std::shared_ptr<atomix::mem::Buffer>> buffers = atomix::mem::mem_route::allocate(1*1024*1024, chunk_size, 64);
    std::vector<std::shared_ptr<atomix::mem::Buffer>> buffers_dest;
    buffers_dest.reserve(buffers.size());

    for (auto& buffer : buffers) {
        memset(buffer->get_begin(), 0xAB, buffer->get_size());
    }


    const std::vector copy_types(buffers.size(), atomix::mem::CopyType::Shallow);
    BENCHMARK("Shallow Copying") {
        constexpr size_t repeat = 1000;
        for (size_t i = 0; i < repeat; i++) {
            buffers_dest = atomix::mem::mem_route::copy(buffers, copy_types); //notice extra implicit move or copy-elision
        }
    };

}


TEST_CASE("Deep Copying", "[buffer]") {
    size_t chunk_size;
    const std::vector<std::shared_ptr<atomix::mem::Buffer>> buffers = atomix::mem::mem_route::allocate(1*1024*1024, chunk_size, 64);
    std::vector<std::shared_ptr<atomix::mem::Buffer>> buffers_dest;
    buffers_dest.reserve(buffers.size());

    for (auto& buffer : buffers) {
        memset(buffer->get_begin(), 0xAB, buffer->get_size());
    }

    const std::vector copy_types(buffers.size(), atomix::mem::CopyType::Deep);
    BENCHMARK("Deep Copying") {
        constexpr size_t repeat = 1000;
        for (size_t i = 0; i < repeat; i++) {
            buffers_dest = atomix::mem::mem_route::copy(buffers, copy_types);
        }
    };

}


TEST_CASE("Moving", "[buffer]") {
    size_t chunk_size;
    std::vector<std::shared_ptr<atomix::mem::Buffer>> buffers = atomix::mem::mem_route::allocate(1*1024*1024, chunk_size, 64);
    std::vector<std::shared_ptr<atomix::mem::Buffer>> buffers_dest;
    buffers_dest.reserve(buffers.size());

    for (auto& buffer : buffers) {
        memset(buffer->get_begin(), 0xAB, buffer->get_size());
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
    size_t chunk_size;
    const std::vector<std::shared_ptr<atomix::mem::Buffer>> buffers = atomix::mem::mem_route::allocate(1*1024*1024, chunk_size, 64);

    for (auto& buffer : buffers) {
        memset(buffer->get_begin(), 0xAB, buffer->get_size());
    }

    uint32_t acc = 0;
    BENCHMARK("Accessing") {
        constexpr size_t repeat = 1000;
        for (size_t i = 0; i < repeat; i++) {
            for (auto& buffer : buffers) {
                for (size_t i = 0; i < buffer->get_size(); i+=4) {
                    uint32_t temp;
                    std::memcpy(&temp, buffer->get_begin() + i,  sizeof(uint32_t));
                    acc += temp;
                }
            }
        }
    };

}

