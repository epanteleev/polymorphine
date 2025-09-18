#include "Slot.h"

#include <ostream>

static std::ostream & operator<<(std::ostream &os, const SlotType &type) {
    switch (type) {
        case SlotType::Byte:   return os << "byte";
        case SlotType::Word:   return os << "word";
        case SlotType::DWord:  return os << "dword";
        case SlotType::QWord:  return os << "qword";
        case SlotType::String: return os << "string";
        default: std::unreachable();
    }
}

void Slot::print_description(std::ostream &os) const {
    const auto vis = [&]<typename T>(const T& value) {
        if constexpr (std::is_same_v<T, std::vector<Slot>>) {
            for (const auto& slot: value) slot.print_description(os);

        } else {
            os << '\t' << '.' << m_type << ' ';
            os << value << std::endl;
        }
    };
    std::visit(vis, m_value);
}