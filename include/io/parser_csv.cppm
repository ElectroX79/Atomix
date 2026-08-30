module;

#include <cstddef>


export module atomix.io.parser_csv;
import atomix.data;
import atomix.io.reader;


export namespace atomix::io {
    class ParserCsv {
        /** @brief It reads the first row, complete the DataTable with the names,
         * and return the size of the first row (including line break).
         *
         * @param reader A class that contains the ptr, the size of the entire file, and the file descriptor
         * @param dt The DataTable to introduce info
         */
        [[nodiscard]] static size_t parse_header(const Reader& reader, DataTable& dt);


    };

    
}



