
module;
#include <vector>
#include <cstdint>
#include <optional>
#include <limits>
#include <bit>
#include <stdexcept>

export module atomix.bit_mask;
import atomix.config;
import atomix.bounds;




export namespace atomix {

    using mask_t = uint64_t;
    constexpr size_t mask_bits = sizeof(mask_t) * 8;

    struct BitPos {
        size_t index;
        size_t bit;

        BitPos(const size_t index1, const size_t bit1)
            : index(index1), bit(bit1) {}

        [[nodiscard]] constexpr size_t to_abs() const {
            return index * mask_bits + bit;
        }

        static constexpr BitPos abs_to_pos(const size_t abs_index) {
            return BitPos{
                abs_index / mask_bits,
                abs_index % mask_bits
            };
        }

        BitPos& operator+=(const size_t n) {
            bit += n;
            index += bit / mask_bits;
            bit %= mask_bits;
            return *this;
        }

        BitPos operator+(const size_t n) const {
            BitPos pos = *this;
            pos += n;
            return pos;
        }

        BitPos& operator-=(const size_t n) {
            const size_t current = to_abs();

            if (current < n) [[unlikely]] {
                throw std::runtime_error("BitPos::minus: current < n");
            }

            const size_t total = current - n;
            index = total / mask_bits;
            bit = total % mask_bits;
            return *this;
        }

        BitPos operator-(const size_t n) const {
            BitPos pos = *this;
            pos -= n;
            return pos;
        }

        std::strong_ordering operator<=>(const BitPos& other) const {
            if (const auto cmp = index <=> other.index; cmp != 0)
                return cmp;

            return bit <=> other.bit;
        }

        std::strong_ordering operator<=>(const size_t other) const {
            if (const auto cmp = index <=> other / mask_bits; cmp != 0)
                return cmp;

            return bit <=> (other % mask_bits);
        }
    };


    class BitMask {
        std::vector<mask_t> mask_ = {};
        size_t n_ones_ = 0;
        size_t abs_size = 0;


        public:

        BitMask() = default;
        explicit BitMask(const size_t size, const bool default_value) {
            size_t resizing = size / mask_bits;
            if (size % mask_bits != 0) resizing++;

            mask_t real_value = 0;
            if (default_value) {
                real_value = std::numeric_limits<mask_t>::max();
            }


            mask_ = std::vector<mask_t>(resizing, real_value);
            abs_size = size;
            n_ones_ = size * static_cast<size_t>(default_value);

        }

        void push_back(const bool value) {
            const size_t index = abs_size;
            abs_size++;

            const size_t word_index = index / mask_bits;
            [[maybe_unused]]const size_t bit_offset = index % mask_bits;


            if (word_index >= mask_.size()) {
                mask_.push_back(0);
            }

            this->set(index, value);

            if (value) n_ones_++;
        }


        //TODO: secure even more this unsafe operation
        std::vector<mask_t>& unsafe_get_mask() {
            return mask_;
        }

        size_t& unsafe_get_n_ones() {
            return n_ones_;
        }

        size_t& unsafe_get_abs_size() {
            return abs_size;
        }

        const std::vector<mask_t>& unsafe_get_mask()const {
            return mask_;
        }

        const size_t& unsafe_get_n_ones()const {
            return n_ones_;
        }

        const size_t& unsafe_get_abs_size() const{
            return abs_size;
        }




        static constexpr size_t to_bits(const BitPos b_pos) {
            return b_pos.index * n_bits_byte + b_pos.bit;
        }

        static constexpr BitPos to_bit_pos(const size_t n_bit){
            return BitPos{n_bit / n_bits_byte, n_bit % n_bits_byte};
        }






        [[nodiscard]]size_t total_bits() const {
            return abs_size;
        }

        [[nodiscard]]size_t n_of_ones() const {
            return n_ones_;
        }

        [[nodiscard]]size_t n_of_zeros() const {
            return total_bits() - n_ones_;
        }

        void clear() {
            mask_.clear();
            n_ones_ = 0;
            abs_size = 0;
        }


        [[nodiscard]] bool operator[](const BitPos bit_pos) const noexcept {
            return mask_[bit_pos.index] & (1ULL << bit_pos.bit);
        }

        [[nodiscard]] bool operator[](const size_t n_bit) const noexcept {
            return (*this)[to_bit_pos(n_bit)];
        }

        [[nodiscard]]bool at(const BitPos bit_pos)const {
            bounds::check_index_individual(to_bits(bit_pos), abs_size);
            return mask_[bit_pos.index] & (1ULL << bit_pos.bit);
        }

        [[nodiscard]]bool at(const size_t n_bit)const{
            return at(to_bit_pos(n_bit));
        }

        [[nodiscard]]std::optional<size_t> first_one() const{
            if (mask_.empty()) return std::nullopt;;

            for (size_t i = 0; i < mask_.size() - 1; ++i) {
                const size_t n = std::countr_zero(mask_[i]);
                if (n != mask_bits)
                    return std::make_optional(
                        to_bits({i,n})
                    );
            }

            const size_t n =
                std::countr_zero(mask_[mask_.size() - 1]);

            if (n + 1 <= to_bit_pos(abs_size).bit)
                return std::make_optional(to_bits({mask_.size() - 1,n}));

            return std::nullopt;

        }


        [[nodiscard]]std::optional<size_t> rfirst_one() const{
            if (mask_.empty()) return std::nullopt;;

            const size_t n =
               std::countl_zero(mask_[0]);

            if (mask_.size() < to_bit_pos(abs_size).bit + n - 1)
                return std::make_optional(to_bits({mask_.size() - 1,n}));


            for (size_t i = 1; i < mask_.size(); ++i) {
                const size_t n = std::countl_zero(mask_[mask_.size() - i]);
                if (n != mask_bits)
                    return std::make_optional(
                        to_bits({i,mask_bits - n})
                    );
            }

            return std::nullopt;
        }


        [[nodiscard]]std::optional<size_t> first_zero()const {
            if (mask_.empty()) return std::nullopt;;

            for (size_t i = 0; i < mask_.size() - 1; ++i) {
                const size_t n = std::countr_one(mask_[i]);
                if (n != mask_bits)
                    return std::make_optional(
                        to_bits({i,n})
                    );
            }

            const size_t n =
                std::countr_one(mask_[mask_.size() - 1]);

            if (n + 1 <= to_bit_pos(abs_size).bit)
                return std::make_optional(to_bits({mask_.size() - 1,n}));

            return std::nullopt;

        }


        [[nodiscard]]std::optional<size_t> rfirst_zero() const{
            if (mask_.empty()) return std::nullopt;;

            const size_t n =
               std::countl_one(mask_[0]);

            if (mask_.size() < to_bit_pos(abs_size).bit + n - 1)
                return std::make_optional(to_bits({mask_.size() - 1,n}));


            for (size_t i = 1; i < mask_.size(); ++i) {
                const size_t n = std::countl_one(mask_[mask_.size() - i]);
                if (n != mask_bits)
                    return std::make_optional(
                        to_bits({i,mask_bits - n})
                    );
            }

            return std::nullopt;
        }


        void set(const size_t bits, const bool value) {
            bounds::check_index_individual(bits, abs_size);
            const auto bit_pos = to_bit_pos(bits);
            //TODO: the conditionals may be removed, study if that worth
            const auto original = mask_[bit_pos.index];
            if (value) {
                mask_[bit_pos.index] |= (1ULL << bit_pos.bit);
                if (original != mask_[bit_pos.index])
                    n_ones_++;
            }
            else {
                mask_[bit_pos.index] &= ~(1ULL << bit_pos.bit);
                if (original != mask_[bit_pos.index])
                    n_ones_--;
            }
        }


        void set_range(const BitPos start, const BitPos end, const bool value) {
            //TODO: this have potential to be optimized
            bounds::check_index_interval(to_bits(start), to_bits(end), abs_size);

            if (value) {
                for (size_t i = to_bits(start); i <= to_bits(end); ++i) {
                    const auto aux = to_bit_pos(i);
                    const auto original = mask_[aux.index];
                    mask_[aux.index] |= (1ULL << aux.bit);
                    if (original != mask_[aux.index])
                        n_ones_++;
                }
            }
            else{
                for (size_t i = to_bits(start); i <= to_bits(end); ++i) {
                    const auto aux = to_bit_pos(i);
                    const auto original = mask_[aux.index];
                    mask_[aux.index] &= ~(1ULL << aux.bit);
                    if (original != mask_[aux.index])
                        n_ones_--;
                }

            }
            
        }












    };
}
