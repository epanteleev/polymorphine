#include "asm/global/Slot.h"
#include "asm/global/Directive.h"

#include <ostream>

static_assert(!std::is_polymorphic_v<aasm::Slot>,"should be");

namespace aasm {
    void Slot::print_description(std::ostream &os) const {
        const auto vis = [&]<typename T>(const T& value) {
            if constexpr (std::is_same_v<T, std::vector<Slot>>) {
                for (const auto& slot: value) slot.print_description(os);

            } else if constexpr (std::is_same_v<T, std::string>) {
                os << '\t' << ".string" << ' ' << '"' << value << '"' << std::endl;

            } else if constexpr (std::is_same_v<T, Constant>) {
                os << '\t' << '.' << to_string(value.type()) << ' ';
                os << value.value() << std::endl;

            } else if constexpr (std::is_same_v<T, const Directive*>) {
                os << '\t' << ".qword " << *value << std::endl;

            } else if constexpr (std::is_same_v<T, ZeroInit>) {
                os << '\t' << ".zero " << value.length() << std::endl;

            } else {
                static_assert(false, "Unsupported type in LIRSlot::print_description");
                std::unreachable();
            }
        };
        std::visit(vis, m_value);
    }
}