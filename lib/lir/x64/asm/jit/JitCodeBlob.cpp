#pragma once

#include "JitCodeBlob.h"

#include <iomanip>
#include <ostream>


std::ostream & operator<<(std::ostream &os, const JitCodeBlob &blob) {
    for (const auto& [name, offset] : blob.m_offset_table) {
        os << name << ": " << std::endl;
        // iterate by 16 bytes
        for (std::size_t i = 0; i < blob.m_size; i += 16) {
            os << '\t' << std::setfill('0') << std::setw(8) << std::hex << (offset + i) << ": ";
            for (std::size_t j = 0; j < 16 && i + j < blob.m_size; ++j) {
                os << std::setfill('0') << std::setw(2) << static_cast<int>(blob.m_buffer[offset + i + j]) << " ";
            }
            os << std::dec << std::endl;
        }
    }
    return os;
}
