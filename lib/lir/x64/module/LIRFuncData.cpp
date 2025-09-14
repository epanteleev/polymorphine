#include "LIRFuncData.h"

#include <iostream>

void LIRFuncData::print(std::ostream &os) const {
    os << m_name << '(';
    for (auto [idx, arg] : std::ranges::enumerate_view(m_args)) {
        if (idx > 0) {
            os << ", ";
        }

        os << arg;
    }
    os << ") ";
    if (!m_global_data.empty()) {
        os << std::endl << "constants: [" << std::endl;
        for (const auto& [idx, data] : std::ranges::enumerate_view(m_global_data)) {
            if (idx > 0) {
                std::cout << std::endl;
            }

            data.second.print_description(os);
        }
        os << "]" << std::endl;
    }
    print_blocks(os);
}
