#include "LIRSlot.h"

#include <ostream>

void LIRSlot::print_description(std::ostream &os) const {
    const auto vis = [&]<typename T>(const T& value) {
        if constexpr (std::is_same_v<T, std::vector<LIRSlot>>) {
            for (const auto& slot: value) slot.print_description(os);

        } else if constexpr (std::is_same_v<T, std::string>) {
            os << '\t' << '.' << to_string(m_type) << ' ' << '"' << value << '"' << std::endl;

        } else {
            os << '\t' << '.' << to_string(m_type) << ' ';
            os << value << std::endl;
        }
    };
    std::visit(vis, m_value);
}