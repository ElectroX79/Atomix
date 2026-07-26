#ifndef SECURITY_CHECK_HPP
#define SECURITY_CHECK_HPP


#include <stdexcept>


namespace atomix::bounds{

     inline void check_index_interval(const size_t begin, const size_t end, const size_t container_size) {
          //always begin >= 0, because begin is size_t
          if (begin > end)[[unlikely]] {
               throw std::invalid_argument("begin must be less than or equal to end");
          }

          if (end > container_size)[[unlikely]] {
               throw std::out_of_range("end must be less than or equal to container size");
          }
     }

     inline void check_index_individual(const size_t index, const size_t container_size) {
          //always index >= 0, because index is size_t
          if (index >= container_size)[[unlikely]] {
               throw std::out_of_range("index must be less than container size");
          }
     }

};

 

#endif