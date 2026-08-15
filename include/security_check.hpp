#ifndef SECURITY_CHECK_HPP
#define SECURITY_CHECK_HPP


#include <iostream>
#include <cassert>
#include <cstdlib>


namespace atomix::bounds{
     //TODO (maybe also to the entire codebase): consider use std::contract (c++26) for precondition and postcondition instead of abort()

     inline void check_index_interval(const size_t begin, const size_t end, const size_t container_size) {
          //always begin >= 0, because begin is size_t
          if (begin > end)[[unlikely]] {
               std::cerr << "Contract violation: begin must be less than or equal to end" << "\n";
               std::abort();
          }

          if (end > container_size)[[unlikely]] {
               std::cerr << "Contract violation: end must be less than or equal to container size" << "\n";
               std::abort();
          }
     }

     inline void check_index_individual(const size_t index, const size_t container_size) {
          //always index >= 0, because index is size_t
          if (index >= container_size)[[unlikely]] {
               std::cerr << "Contract violation: index must be less than container size";
               std::abort();
          }
     }

};

 

#endif