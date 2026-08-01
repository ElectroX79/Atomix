#include <stdexcept>
#include <cstdint>

#include <vector>

#include "../external/catch2/catch_amalgamated.hpp"
#include "mem_route.hpp"
#include "buffer.hpp"
namespace {
    void check_integrity(const size_t size) {
        auto buffers = atomix::mem::mem_route::allocate(size, 64);

        REQUIRE_FALSE(buffers.empty());

        size_t total = 0;


        // Every returned buffer must be valid and contribute to the requested size.
        for (const auto& buffer : buffers) {
            REQUIRE(buffer != nullptr);
            CHECK(buffer->get_begin() != nullptr);
            CHECK(buffer->size() > 0);

            total += buffer->size();
        }

        CHECK(total == size);

        // Consecutive buffers must map a contiguous memory region.
        for (size_t i = 1; i < buffers.size(); ++i) {
            CHECK(
                buffers[i - 1]->get_begin() + buffers[i - 1]->size()
                == buffers[i]->get_begin()
            );
        }

        // Ensure every byte in every segment is writable.
        for (auto& buffer : buffers) {
            std::memset(
                buffer->get_begin(),
                0xAB,
                buffer->size()
            );
        }
    }
}


TEST_CASE("Buffer") {
    constexpr size_t kb = 1024;
    constexpr size_t mb = 1024*kb;
    [[maybe_unused]] const size_t gb = 1024*mb;

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
        auto buffers = atomix::mem::mem_route::allocate(200 * kb, 64);

        REQUIRE(buffers.size() > 1);

        const auto survivor = buffers.back();
        // Destroy every buffer except the last one.
        buffers.erase(buffers.begin(), buffers.end() - 1);

        // The surviving buffer should still own valid writable memory.
        std::memset(
            survivor->get_begin(),
            42,
            survivor->size()
        );
    }

    SECTION("Buffers can be released in arbitrary order") {
        auto buffers = atomix::mem::mem_route::allocate(10*mb);

        buffers.erase(buffers.begin() + 1);
        buffers.pop_back();
        buffers.erase(buffers.begin());
        buffers.clear();
    }

}