#include <ostream>

#include "NamedSlot.h"

#include <utility>

std::ostream & operator<<(std::ostream &os, const SlotType &type) {
    switch (type) {
        case SlotType::Byte:   return os << "byte";
        case SlotType::Word:   return os << "word";
        case SlotType::DWord:  return os << "dword";
        case SlotType::QWord:  return os << "qword";
        case SlotType::String: return os << "string";
        default: std::unreachable();
    }
}

std::uint8_t NamedSlot::size() const noexcept {
    switch (m_type) {
        case SlotType::Byte:  return 1;
        case SlotType::Word:  return 2;
        case SlotType::DWord: return 4;
        case SlotType::QWord: return 8;
        case SlotType::String: return std::get<std::string>(m_value).size();
        default: std::unreachable();
    }
}

void NamedSlot::print_description(std::ostream &os) const {
    const auto vis = [&]<typename T>(const T& value) {
        os << value;
    };
    os << m_name << ':' << std::endl;
    os << '\t' << '.' << m_type << ' ';
    std::visit(vis, m_value);
    os << std::endl;
}

std::ostream &operator<<(std::ostream &os, const NamedSlot &slot) {
    return os << '$' << slot.m_name;
}