module;

#include <cstdint>
#include <expected>
#include <vector>
#include <bit>
#include <cstddef>
#include <optional>

export module atomix.mem.arena_allocator;
import atomix.mem.vmem;
import atomix.config;


namespace atomix::mem {
    class ArenaAllocator {
        struct Pos {
            size_t arenas_index;
            size_t in_arena_index;
        };
        struct Arena {
            uint8_t* begin;
            size_t size = arena_allocator::arena_size;
            uint64_t mask[arena_allocator::arena_size/(arena_allocator::chunk_size * 8 * sizeof(uint64_t))];

            Arena(){
                begin = vmem::allocate(arena_allocator::arena_size);
                for (auto& x : mask) {
                    x = 0;
                }
            }

            ~Arena(){
                vmem::deallocate(begin, size);
            }

            Arena(const Arena&) = delete;
            Arena& operator=(const Arena&) = delete;
            Arena(Arena&&) = delete;
            Arena& operator=(Arena&&) = delete;
        };

        std::vector<Arena> arenas;
        std::vector<uint64_t> arenas_max_size;

        explicit ArenaAllocator() {
            arenas_max_size =  std::vector<uint64_t> (arena_allocator::initial_size / arena_allocator::arena_size, arena_allocator::max_size);
            arenas.reserve(arena_allocator::initial_size / arena_allocator::arena_size);
            for (size_t i = 0; i < arena_allocator::initial_size / arena_allocator::arena_size; ++i) {
                arenas.emplace_back();
            }
        }

        static ArenaAllocator& instance(){
            static ArenaAllocator instance;
            return instance;
        }

        void update_arenas_max_size(const size_t index) {

        }
        std::optional<size_t> vector_find(const size_t size){
            size_t index = 0;
            for (const auto& x : instance().arenas_max_size) {
                if (x >= size) {
                    return index;
                }
                ++index;
            }
            return std::nullopt;
        }

        std::optional<size_t> vector_bit_find(const size_t index, const size_t size){
           
        }

        std::optional<Pos> find_free_space(const size_t size){
            Pos pos;
            auto arenas_index = vector_bit_find(size);
            if (arenas_index.has_value()){
                auto in_arena_index = vector_bit_find(arenas_index.value(), size);
                if (in_arena_index.has_value()){
                    pos.arenas_index = arenas_index.value();
                    pos.in_arena_index = in_arena_index.value();
                    return pos;
                }
            }
            return std::nullopt;
        }



    public:

        ~ArenaAllocator() = default; //TODO: Also temporal, remember change if it needs
        ArenaAllocator(const ArenaAllocator&) = delete;
        ArenaAllocator& operator=(const ArenaAllocator&) = delete;
        ArenaAllocator(const ArenaAllocator&&) = delete;
        ArenaAllocator& operator=(const ArenaAllocator&&) = delete;


        void* allocate(size_t size, size_t alignment) {

        }
        void deallocate(void* ptr, size_t size);





    };
}