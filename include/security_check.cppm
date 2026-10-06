
module;
#include <iostream>
#include <cstdlib>
#include <stdexcept>


export module atomix.bounds;


export namespace atomix::bounds{
     //TODO (maybe also to the entire codebase): consider use std::contract (c++26) for precondition and postcondition

     inline void check_index_interval(const size_t begin, const size_t end, const size_t container_size) {
          //always begin >= 0, because begin is size_t
          if (begin > end)[[unlikely]] {
               throw (std::out_of_range("Contract violation: begin must be less than or equal to end"));
          }

          if (end > container_size)[[unlikely]] {
               throw(std::out_of_range("Contract violation: end must be less than or equal to container size"));
          }
     }

     inline void check_index_individual(const size_t index, const size_t container_size) {
          //always index >= 0, because index is size_t
          if (index >= container_size)[[unlikely]] {
               throw(std::out_of_range("Contract violation: index must be less than container size"));
          }
     }
     
     inline void check_offset_length (const size_t offset, const size_t length, const size_t container_size) {
          if (offset > container_size || length > (container_size - offset))[[unlikely]]{
               throw std::out_of_range("Contract violation: offset + length exceeds container size (or overflow)");
          }

     }

};

