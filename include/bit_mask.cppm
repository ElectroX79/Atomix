
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
    constexpr size_t mask_t_bits = sizeof(mask_t) * 8;

    struct BitPos {
        size_t index;
        size_t bit;

        BitPos(const size_t index1, const size_t bit1)
            : index(index1), bit(bit1) {}

        [[nodiscard]] constexpr size_t to_abs() const {
            return index * mask_t_bits + bit;
        }

        static constexpr BitPos abs_to_pos(const size_t abs_index) {
            return BitPos{
                abs_index / mask_t_bits,
                abs_index % mask_t_bits
            };
        }

        BitPos& operator+=(const size_t n) {
            bit += n;
            index += bit / mask_t_bits;
            bit %= mask_t_bits;
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
            index = total / mask_t_bits;
            bit = total % mask_t_bits;
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
            if (const auto cmp = index <=> other / mask_t_bits; cmp != 0)
                return cmp;

            return bit <=> (other % mask_t_bits);
        }
    };


    class BitMask {
        std::vector<mask_t> mask_ = {};
        size_t n_ones_ = 0;
        size_t abs_size = 0;





        public:

        BitMask() = default;
        explicit BitMask(const size_t size, const bool default_value) {
            size_t resizing = size / mask_t_bits;
            if (size % mask_t_bits != 0) resizing++;

            mask_t real_value = 0;
            if (default_value) {
                real_value = std::numeric_limits<mask_t>::max();
            }


            mask_ = std::vector<mask_t>(resizing, real_value);
            abs_size = size;
            n_ones_ = size * static_cast<size_t>(default_value);

            if (default_value && (size % mask_t_bits != 0)) {
                const size_t rem = size % mask_t_bits;
                mask_.back() &= ~((std::numeric_limits<mask_t>::max()) << rem);
            }

        }

        void push_back(const bool value) {
            const size_t index = abs_size;
            abs_size++;

            const size_t word_index = index / mask_t_bits;
            [[maybe_unused]]const size_t bit_offset = index % mask_t_bits;


            if (word_index >= mask_.size()) {
                mask_.push_back(0);
            }

            this->set(index, value);

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
            return b_pos.index * mask_t_bits + b_pos.bit;
        }

        static constexpr BitPos to_bit_pos(const size_t n_bit){
            return BitPos{n_bit / mask_t_bits, n_bit % mask_t_bits};
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


        [[nodiscard]] std::optional<size_t> first_one() const {
            if ( abs_size == 0 or n_ones_ == 0) return std::nullopt;

            const size_t rem = to_bit_pos(abs_size).bit;

            for (size_t i = 0; i < mask_.size(); ++i) {
                mask_t word = mask_[i];
                if (i == mask_.size() - 1 && rem != 0) {
                    word &= ~((std::numeric_limits<mask_t>::max()) << rem);
                }

                if (word != 0) {
                    const size_t bit = std::countr_zero(word);
                    return to_bits({i, bit});
                }

            }

            return std::nullopt;

        }


        [[nodiscard]] std::optional<size_t> rfirst_one() const {
            if ( abs_size == 0 or n_ones_ == 0) return std::nullopt;

            constexpr mask_t max_value = std::numeric_limits<mask_t>::max();
            const size_t rem = to_bit_pos(abs_size).bit;

            const mask_t tail_mask = (rem == 0) ? max_value : ~((max_value) << rem);

            for (size_t i = mask_.size(); i > 0; --i) {
                const size_t word_idx = i - 1;
                mask_t word = mask_[word_idx];

                if (word_idx == mask_.size() - 1) {
                    word &= tail_mask;
                }


                if (word != 0) {
                    const size_t bit = (mask_t_bits - 1) - std::countl_zero(word);
                    return to_bits({word_idx, bit});
                }
            }

            return std::nullopt;
        }


        [[nodiscard]] std::optional<size_t> first_zero() const {
            if (abs_size == 0 or n_of_zeros() == 0) return std::nullopt;

            const size_t rem = to_bit_pos(abs_size).bit;

            for (size_t i = 0; i < mask_.size(); ++i) {
                mask_t word = mask_[i];
                if (i == mask_.size() - 1 && rem != 0) {
                    word |= ((std::numeric_limits<mask_t>::max()) << rem);
                }

                if (word != std::numeric_limits<mask_t>::max()) {
                    const size_t bit = std::countr_one(word);
                    return to_bits({i, bit});
                }
            }
            return std::nullopt;
        }


        [[nodiscard]]std::optional<size_t> rfirst_zero() const{
            if (abs_size == 0 or n_of_zeros() == 0) return std::nullopt;

            constexpr mask_t max_value = std::numeric_limits<mask_t>::max();
            const size_t rem = to_bit_pos(abs_size).bit;

            const mask_t tail_mask = (rem == 0) ? 0 : ((max_value) << rem);

            for (size_t i = mask_.size(); i > 0; --i) {
                const size_t word_idx = i - 1;
                mask_t word = mask_[word_idx];

                if (word_idx == mask_.size() - 1) {
                    word |= tail_mask;
                }


                if (word != max_value) {
                    const size_t bit = (mask_t_bits - 1) - std::countl_one(word);
                    return to_bits({word_idx, bit});
                }
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
            //TODO: this could potential to be optimized
            bounds::check_index_interval(to_bits(start), to_bits(end), abs_size);

            if (value) {
                for (size_t i = to_bits(start); i < to_bits(end); ++i) {
                    const auto aux = to_bit_pos(i);
                    const auto original = mask_[aux.index];
                    mask_[aux.index] |= (1ULL << aux.bit);
                    if (original != mask_[aux.index])
                        n_ones_++;
                }
            }
            else{
                for (size_t i = to_bits(start); i < to_bits(end); ++i) {
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
