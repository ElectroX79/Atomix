#include <stdexcept>
#include <cstdint>
#include <utility>
#include <vector>

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
    void check_integrity(const size_t size) {

        auto buffers = create_buffers(size);

        REQUIRE_FALSE(buffers.empty());

        size_t total = 0;


        // Every returned buffer must be valid and contribute to the requested size.
        for (const auto& buffer : buffers) {
            REQUIRE(!buffer.non_owner());
            CHECK(buffer.get_begin() != nullptr);
            CHECK(buffer.get_size() > 0);

            total += buffer.get_size();
        }

        CHECK(total == size);

        // Ensure every byte in every segment is writable.
        for (auto& buffer : buffers) {
            std::memset(
                buffer.get_begin(),
                0xAB,
                buffer.get_size()
            );
        }
    }
}


TEST_CASE("Buffer") {
    constexpr size_t kb = 1024;
    constexpr size_t mb = 1024*kb;
    [[maybe_unused]] constexpr size_t gb = 1024*mb;

    SECTION("Allocates the requested memory as contiguous buffer segments, small size") {
        check_integrity(2*kb);
    }

    SECTION("Allocates the requested memory as contiguous buffer segments, small size") {
        check_integrity(512*kb);
    }

    SECTION("Allocates the requested memory as contiguous buffer segments, small size") {
        check_integrity(32*mb);
    }

    SECTION("Destroying some buffers keeps the remaining ones valid"){

        auto buffers = create_buffers(200 * atomix::units::KiB);

        REQUIRE(buffers.size() > 1);

        const auto survivor = buffers.back();
        // Destroy every buffer except the last one.
        buffers.erase(buffers.begin(), buffers.end() - 1);

        // The surviving buffer should still own valid writable memory.
        std::memset(
            survivor.get_begin(),
            42,
            survivor.get_size()
        );
    }

    SECTION("Buffers can be released in arbitrary order") {

        auto buffers = create_buffers(10 * atomix::units::MiB);

        buffers.erase(buffers.begin() + 1);
        buffers.pop_back();
        buffers.erase(buffers.begin());
        buffers.clear();
    }

}
/*
TEST_CASE("Copying") {
    constexpr size_t kb = 1024;
    constexpr size_t mb = 1024*kb;
    [[maybe_unused]] constexpr size_t gb = 1024*mb;

    size_t chunk_size;

    std::vector<std::shared_ptr<atomix::mem::Buffer>> buffers = atomix::mem::mem_route::allocate(mb, chunk_size, 64);

    for (auto& buffer : buffers) {
        memset(buffer->get_begin(), 0xAB, buffer->get_size());
    }



    SECTION("Full deep copy") {
        std::vector copy_types(buffers.size(), atomix::mem::CopyType::Deep);
        const auto aux = atomix::mem::mem_route::copy(buffers, copy_types);

        CHECK(buffers.size() == aux.size());
        for (size_t i = 0; i < buffers.size(); ++i) {
            CHECK(memcmp(buffers[i]->get_begin(), aux[i]->get_begin(), buffers[i]->get_size()) == 0);
            CHECK(buffers[i] != aux[i]);

            std::memset(aux[i]->get_begin(), 0x11, aux[i]->get_size());
            CHECK(memcmp(buffers[i]->get_begin(), aux[i]->get_begin(), buffers[i]->get_size()) != 0);

        }

    }

    SECTION("Full shallow copy") {
        std::vector copy_types(buffers.size(), atomix::mem::CopyType::Shallow);
        const auto aux = atomix::mem::mem_route::copy(buffers, copy_types);

        CHECK(buffers.size() == aux.size());
        for (size_t i = 0; i < buffers.size(); ++i) {
            CHECK(memcmp(buffers[i]->get_begin(), aux[i]->get_begin(), buffers[i]->get_size()) == 0);
            CHECK(buffers[i] == aux[i]);

            std::memset(aux[i]->get_begin(), 0x11, aux[i]->get_size());
            CHECK(memcmp(buffers[i]->get_begin(), aux[i]->get_begin(), buffers[i]->get_size()) == 0);
        }
    }

    SECTION("Full reinitialize copy") {
        std::vector copy_types(buffers.size(), atomix::mem::CopyType::Reinitialize);
        const auto aux = atomix::mem::mem_route::copy(buffers, copy_types);

        CHECK(buffers.size() == aux.size());
        for (size_t i = 0; i < buffers.size(); ++i) {
            CHECK(buffers[i] != aux[i]);
        }

    }

    SECTION("Mixed") {
        //pattern: deep, shallow, reinitialize
        std::vector<atomix::mem::CopyType> copy_types(buffers.size());
        for (size_t i = 0; i < buffers.size(); ++i) {
            switch (i % 3) {
                case 0:
                    copy_types[i] = atomix::mem::CopyType::Deep;
                    break;
                case 1:
                    copy_types[i] = atomix::mem::CopyType::Shallow;
                    break;
                case 2:
                    copy_types[i] = atomix::mem::CopyType::Reinitialize;
                    break;
                default:
                    std::abort();
            }
        }
        const auto aux = atomix::mem::mem_route::copy(buffers, copy_types);

        CHECK(buffers.size() == aux.size());
        for (size_t i = 0; i < buffers.size(); ++i) {
            switch (i % 3) {
                case 0: //deep
                    CHECK(memcmp(buffers[i]->get_begin(), aux[i]->get_begin(), buffers[i]->get_size()) == 0);
                    CHECK(buffers[i] != aux[i]);

                    std::memset(aux[i]->get_begin(), 0x11, aux[i]->get_size());
                    CHECK(memcmp(buffers[i]->get_begin(), aux[i]->get_begin(), buffers[i]->get_size()) != 0);
                    break;
                case 1: //shallow
                    CHECK(memcmp(buffers[i]->get_begin(), aux[i]->get_begin(), buffers[i]->get_size()) == 0);
                    CHECK(buffers[i] == aux[i]);

                    std::memset(aux[i]->get_begin(), 0x11, aux[i]->get_size());
                    CHECK(memcmp(buffers[i]->get_begin(), aux[i]->get_begin(), buffers[i]->get_size()) == 0);
                    break;
                case 2: //reinitialize
                    CHECK(buffers[i] != aux[i]);
                    break;
                default:
                    std::abort();
            }

        }

    }







}
*/