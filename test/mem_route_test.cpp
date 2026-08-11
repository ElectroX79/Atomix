#include <stdexcept>
#include <cstdint>

#include <vector>

#include "../external/catch2/catch_amalgamated.hpp"
#include "mem/mem_route.hpp"
#include "mem/buffer.hpp"
namespace {
    void check_integrity(const size_t size) {

        size_t chunk_size[[maybe_unused]] = 64*1024; //64kb
        auto buffers = atomix::mem::mem_route::allocate(size, chunk_size, 64);

        REQUIRE_FALSE(buffers.empty());

        size_t total = 0;


        // Every returned buffer must be valid and contribute to the requested size.
        for (const auto& buffer : buffers) {
            REQUIRE(buffer != nullptr);
            CHECK(buffer->get_begin() != nullptr);
            CHECK(buffer->get_size() > 0);

            total += buffer->get_size();
        }

        CHECK(total == size);

        // Ensure every byte in every segment is writable.
        for (auto& buffer : buffers) {
            std::memset(
                buffer->get_begin(),
                0xAB,
                buffer->get_size()
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
        size_t chunk_size[[maybe_unused]] = 64*1024; //64kb
        auto buffers = atomix::mem::mem_route::allocate(200 * kb, chunk_size, 64);

        REQUIRE(buffers.size() > 1);

        const auto survivor = buffers.back();
        // Destroy every buffer except the last one.
        buffers.erase(buffers.begin(), buffers.end() - 1);

        // The surviving buffer should still own valid writable memory.
        std::memset(
            survivor->get_begin(),
            42,
            survivor->get_size()
        );
    }

    SECTION("Buffers can be released in arbitrary order") {
        size_t chunk_size[[maybe_unused]] = 64*1024; //64kb
        auto buffers = atomix::mem::mem_route::allocate(10*mb,chunk_size, 64);

        buffers.erase(buffers.begin() + 1);
        buffers.pop_back();
        buffers.erase(buffers.begin());
        buffers.clear();
    }

}