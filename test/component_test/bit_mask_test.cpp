#include <../external/catch2/catch_amalgamated.hpp>
#include <optional>
#include <stdexcept>

import atomix.bit_mask;
using namespace atomix;

TEST_CASE("BitPos - Conversion, arithmetic, and ordering", "[BitPos]") {
    SECTION("Bidirectional conversion between absolute index and BitPos") {
        constexpr size_t abs_idx = 130; // Word 2 (64 * 2 = 128), bit 2
        const auto pos = BitPos::abs_to_pos(abs_idx);
        CHECK(pos.index == 2);
        CHECK(pos.bit == 2);
        CHECK(pos.to_abs() == abs_idx);
    }

    SECTION("Basic arithmetic and word boundary crossing") {
        BitPos pos(0, 62);
        pos += 5; // Crosses into word 1, bit 3
        CHECK(pos.index == 1);
        CHECK(pos.bit == 3);

        pos -= 4; // Moves back to word 0, bit 63
        CHECK(pos.index == 0);
        CHECK(pos.bit == 63);

        CHECK_THROWS_AS(pos - 100, std::runtime_error);
    }

    SECTION("Three-way comparison (<=>)") {
        BitPos p1(0, 10);
        BitPos p2(0, 20);
        BitPos p3(1, 0);

        CHECK(p1 < p2);
        CHECK(p2 < p3);
        CHECK((p1 <=> 10) == std::strong_ordering::equal);
        CHECK((p3 <=> 64) == std::strong_ordering::equal);
    }
}

TEST_CASE("BitMask - Construction and counts", "[BitMask]") {
    SECTION("Default empty mask") {
        BitMask mask;
        CHECK(mask.total_bits() == 0);
        CHECK(mask.n_of_ones() == 0);
        CHECK(mask.n_of_zeros() == 0);
        CHECK(mask.first_one() == std::nullopt);
        CHECK(mask.first_zero() == std::nullopt);
    }

    SECTION("Initialize to false across multiple words") {
        BitMask mask(130, false);
        CHECK(mask.total_bits() == 130);
        CHECK(mask.n_of_ones() == 0);
        CHECK(mask.n_of_zeros() == 130);
        CHECK(mask[0] == false);
        CHECK(mask[129] == false);
    }

    SECTION("Initialize to true and clear trailing padding bits") {
        BitMask mask(70, true);
        CHECK(mask.total_bits() == 70);
        CHECK(mask.n_of_ones() == 70);
        CHECK(mask.n_of_zeros() == 0);
        CHECK(mask[69] == true);

        // Ensure no false zeros are detected in the padded capacity
        CHECK(mask.first_zero() == std::nullopt);
        CHECK(mask.rfirst_zero() == std::nullopt);
    }
}

TEST_CASE("BitMask - Bit mutation and expansion", "[BitMask]") {
    BitMask mask(100, false);

    SECTION("set() mutation and count tracking") {
        mask.set(5, true);
        mask.set(65, true);

        CHECK(mask[5] == true);
        CHECK(mask[65] == true);
        CHECK(mask[6] == false);
        CHECK(mask.n_of_ones() == 2);
        CHECK(mask.n_of_zeros() == 98);

        // Idempotency: re-setting true does not increment counter
        mask.set(5, true);
        CHECK(mask.n_of_ones() == 2);

        mask.set(5, false);
        CHECK(mask[5] == false);
        CHECK(mask.n_of_ones() == 1);
    }

    SECTION("dynamic push_back crossing 64-bit boundaries") {
        BitMask dynamic_mask(63, false);
        CHECK(dynamic_mask.unsafe_get_mask().size() == 1);

        dynamic_mask.push_back(true);  // bit 63 (fills word 0)
        dynamic_mask.push_back(true);  // bit 64 (allocates word 1)

        CHECK(dynamic_mask.total_bits() == 65);
        CHECK(dynamic_mask.unsafe_get_mask().size() == 2);
        CHECK(dynamic_mask[63] == true);
        CHECK(dynamic_mask[64] == true);
        CHECK(dynamic_mask.n_of_ones() == 2);
    }

    SECTION("Half-open range set_range [first, last)") {
        // [60, 68) sets bits 60..67, crossing word 0 to word 1
        mask.set_range(BitMask::to_bit_pos(60), BitMask::to_bit_pos(68), true);

        CHECK(mask.n_of_ones() == 8);
        CHECK(mask[59] == false);
        CHECK(mask[60] == true);
        CHECK(mask[67] == true);
        CHECK(mask[68] == false);

        // Clear sub-range [62, 66) -> bits 62, 63, 64, 65
        mask.set_range(BitMask::to_bit_pos(62), BitMask::to_bit_pos(66), false);
        CHECK(mask.n_of_ones() == 4);
        CHECK(mask[61] == true);
        CHECK(mask[62] == false);
        CHECK(mask[65] == false);
        CHECK(mask[66] == true);

        // Empty range [10, 10) is a no-op
        mask.set_range(BitMask::to_bit_pos(10), BitMask::to_bit_pos(10), true);
        CHECK(mask.n_of_ones() == 4);
    }

    SECTION("clear() resets storage and state") {
        mask.set(10, true);
        mask.clear();
        CHECK(mask.total_bits() == 0);
        CHECK(mask.n_of_ones() == 0);
        CHECK(mask.unsafe_get_mask().empty());
    }
}

TEST_CASE("BitMask - Forward and reverse bit scans (first/rfirst)", "[BitMask]") {
    SECTION("Homogeneous masks") {
        BitMask all_zeros(130, false);
        CHECK(all_zeros.first_one() == std::nullopt);
        CHECK(all_zeros.rfirst_one() == std::nullopt);
        CHECK(all_zeros.first_zero() == 0);
        CHECK(all_zeros.rfirst_zero() == 129);

        BitMask all_ones(130, true);
        CHECK(all_ones.first_one() == 0);
        CHECK(all_ones.rfirst_one() == 129);
        CHECK(all_ones.first_zero() == std::nullopt);
        CHECK(all_ones.rfirst_zero() == std::nullopt);
    }

    SECTION("Scanning across multiple 64-bit words") {
        BitMask mask(130, false); // Words 0 (0..63), 1 (64..127), 2 (128..129)
        mask.set(12, true);
        mask.set(70, true);
        mask.set(128, true);

        CHECK(mask.first_one() == 12);
        CHECK(mask.rfirst_one() == 128);

        mask.set(128, false);
        CHECK(mask.rfirst_one() == 70); // Falls back to previous word

        mask.set(70, false);
        CHECK(mask.rfirst_one() == 12); // Falls back to word 0
    }

    SECTION("Edge cases on exact multiples of 64 (rem == 0)") {
        BitMask mask(64, true); // Exactly one 64-bit word
        mask.set(63, false);    // Clear MSB
        mask.set(0, false);     // Clear LSB

        CHECK(mask.first_zero() == 0);
        CHECK(mask.rfirst_zero() == 63);

        CHECK(mask.first_one() == 1);
        CHECK(mask.rfirst_one() == 62);
    }

    SECTION("Ignores unused padding bits beyond abs_size") {
        BitMask mask(66, false); // Only bits 64 and 65 are valid in word 1
        mask.set(65, true);

        // Must find 65 and not trigger false positives on padding bits 66..127
        CHECK(mask.rfirst_one() == 65);
        CHECK(mask.first_one() == 65);
        CHECK(mask.rfirst_zero() == 64);
    }
}