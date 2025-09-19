#include "asm/global/Slot.h"
#include "asm/global/Directive.h"

#include <ostream>

namespace aasm {
    void Slot::print_description(std::ostream &os) const {
        const auto vis = [&]<typename T>(const T& value) {
            if constexpr (std::is_same_v<T, std::vector<Slot>>) {
                for (const auto& slot: value) slot.print_description(os);

            } else if constexpr (std::is_same_v<T, std::string>) {
                os << '\t' << '.' << to_string(m_type) << ' ' << '"' << value << '"' << std::endl;

            } else if constexpr (std::is_same_v<T, const Directive*>) {
                os << '\t' << '.' << to_string(m_type) << ' ' << *value << std::endl;

            } else {
                os << '\t' << '.' << to_string(m_type) << ' ';
                os << value << std::endl;
            }
        };
        std::visit(vis, m_value);
    }
}