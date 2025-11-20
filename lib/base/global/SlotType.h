#pragma once

#include <cstdint>
#include <utility>
#include <string_view>


enum class SlotType: std::uint8_t {
    Byte,
    Word,
    DWord,
    QWord,
};

inline SlotType to_slot_type(const std::uint8_t size) {
    switch (size) {
        case 1:  return SlotType::Byte;
        case 2:  return SlotType::Word;
        case 4:  return SlotType::DWord;
        case 8:  return SlotType::QWord;
        default: std::unreachable();
    }
}

inline std::string_view to_string(const SlotType &type) {
    switch (type) {
        case SlotType::Byte:   return "byte";
        case SlotType::Word:   return "word";
        case SlotType::DWord:  return "dword";
        case SlotType::QWord:  return "qword";
        default: std::unreachable();
    }
}