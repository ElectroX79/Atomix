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

        auto buffers = create_buffers(200 * atomix::KiB);

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

        auto buffers = create_buffers(10 * atomix::MiB);

        buffers.erase(buffers.begin() + 1);
        buffers.pop_back();
        buffers.erase(buffers.begin());
        buffers.clear();
    }

}
