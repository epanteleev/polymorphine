#pragma once

#include <cstdint>
#include <iosfwd>
#include <optional>
#include <string>
#include <utility>
#include <variant>
#include <vector>

#include "utility/Error.h"

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

class Slot;

template<typename T>
concept SlotVariant = std::convertible_to<T, std::int64_t> ||
    std::convertible_to<T, std::string_view> ||
    std::convertible_to<T, std::vector<Slot>>;

class Slot final {
public:
    template<SlotVariant T>
    explicit Slot(T&& data, const SlotType type) noexcept:
        m_type(type),
        m_size(compute_size(data, type)),
        m_value(std::forward<T>(data)) {}

    [[nodiscard]]
    SlotType type() const noexcept {
        return m_type;
    }

    template<typename Fn>
    decltype(auto) visit(Fn&& fn) const {
        return std::visit(std::forward<Fn>(fn), m_value);
    }

    [[nodiscard]]
    std::uint8_t size() const noexcept {
        return m_size;
    }

    void print_description(std::ostream &os) const;

private:
    template<SlotVariant T>
    static std::size_t compute_size(const T& data, const SlotType type) noexcept {
        if constexpr (std::is_same_v<T, std::string>) {
            return data.size();

        } else if constexpr (std::is_same_v<T, std::int64_t>) {
            switch (type) {
                case SlotType::Byte:  return 1;
                case SlotType::Word:  return 2;
                case SlotType::DWord: return 4;
                case SlotType::QWord: return 8;
                default: std::unreachable();
            }

        } else if constexpr (std::is_same_v<T, std::vector<Slot>>) {
            assertion(type == SlotType::Aggregate, "Type must be Aggregate for vector<Slot>");
            std::size_t total_size{};
            for (const auto& slot: data) {
                total_size += slot.size();
            }
            return total_size;

        } else {
            static_assert(false, "Unsupported type in Slot::compute_size");
            std::unreachable();
        }
    }

    SlotType m_type;
    std::size_t m_size;
    std::variant<std::int64_t, std::string, std::vector<Slot>> m_value;
};