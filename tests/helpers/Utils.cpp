#include "Utils.h"

#include <iomanip>
#include <iostream>

void print_hex(const std::uint8_t *data, std::size_t size) {
    std::cout << "Hex: ";
    for (std::size_t i = 0; i < size; ++i)
        std::cout << std::hex << std::setw(2) << std::setfill('0')
                  << static_cast<int>(data[i]) << " ";
    std::cout << std::dec << std::endl;
}

std::string make_string(const aasm::AsmBuffer &a) {
    std::ostringstream os;
    os << a;
    return os.str();
}