#pragma once

#include <cstdint>
#include <optional>


namespace aasm {
    enum class SlotType: std::uint8_t {
        Byte,
        Word,
        DWord,
        QWord,
        String,
        Aggregate,
    };

    inline std::optional<SlotType> to_slot_type(const std::uint8_t size) {
        switch (size) {
            case 1: return SlotType::Byte;
            case 2: return SlotType::Word;
            case 4: return SlotType::DWord;
            case 8: return SlotType::QWord;
            default: return std::nullopt;
        }
    }

    inline std::string_view to_string(const SlotType &type) {
        switch (type) {
            case SlotType::Byte:   return "byte";
            case SlotType::Word:   return "word";
            case SlotType::DWord:  return "dword";
            case SlotType::QWord:  return "qword";
            case SlotType::String: return "string";
            default: std::unreachable();
        }
    }
}