#ifndef PARSER_CSV_HPP
#define PARSER_CSV_HPP

#include "reader.hpp"
#include "../data/data_table.hpp"

namespace atomix::io::parser_csv {
    /** @brief It reads the first row, complete the DataTable with the names,
     * and return the size of the first row (including line break).
     *
     * @param reader A class that contains the ptr, the size of the entire file, and the file descriptor
     * @param dt The DataTable to introduce info
     */
    [[nodiscard]]size_t parse_header(const Reader& reader, DataTable& dt);

    
}



#endif
