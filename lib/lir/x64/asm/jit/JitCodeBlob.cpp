#include "JitCodeBlob.h"

#include <iomanip>
#include <ostream>


std::ostream & operator<<(std::ostream &os, const JitCodeBlob &blob) {
    for (const auto& [name, code_chunk] : blob.m_offset_table) {
        os << name->name() << ": " << std::endl;
        // iterate by 16 bytes
        for (std::size_t i{}; i < code_chunk.size; i += 16) {
            os << std::setfill('0') << std::setw(8) << std::hex << (code_chunk.offset + i) << ": ";
            for (std::size_t j{}; j < 16 && i + j < code_chunk.size; j++) {
                os << std::setfill('0') << std::setw(2) << static_cast<int>(blob.m_code_buffer[code_chunk.offset + i + j]) << " ";
            }
            os << std::dec << std::endl;
        }
    }
    return os;
}
