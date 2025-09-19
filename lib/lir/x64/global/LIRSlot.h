#pragma once

#include <cstdint>
#include <iosfwd>
#include <optional>
#include <string>
#include <utility>
#include <variant>
#include <vector>

#include "asm/global/Slot.h"
#include "utility/Error.h"
#include "lir/x64/lir_frwd.h"

template<typename T>
concept SlotVariant = std::convertible_to<T, std::int64_t> ||
    std::convertible_to<T, std::string_view> ||
    std::convertible_to<T, std::vector<LIRSlot>> ||
    std::convertible_to<T, const LIRNamedSlot*>;

class LIRSlot final {
public:
    template<typename T>
    explicit LIRSlot(T&& data, const aasm::SlotType type) noexcept:
        m_type(type),
        m_size(compute_size(data, type)),
        m_value(std::forward<T>(data)) {}

    [[nodiscard]]
    aasm::SlotType type() const noexcept {
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
    template<typename T>
    static std::size_t compute_size(const T& data, const aasm::SlotType type) noexcept {
        if constexpr (std::is_same_v<T, std::string>) {
            assertion(type == aasm::SlotType::String, "Type must be String for std::string");
            return data.size();

        } else if constexpr (std::is_same_v<T, std::int64_t>) {
            switch (type) {
                case aasm::SlotType::Byte:  return 1;
                case aasm::SlotType::Word:  return 2;
                case aasm::SlotType::DWord: return 4;
                case aasm::SlotType::QWord: return 8;
                default: std::unreachable();
            }

        } else if constexpr (std::is_same_v<T, std::vector<LIRSlot>>) {
            assertion(type == aasm::SlotType::Aggregate, "Type must be Aggregate for vector<Slot>");
            std::size_t total_size{};
            for (const auto& slot: data) {
                total_size += slot.size();
            }
            return total_size;

        } else if constexpr (std::is_same_v<T, const LIRNamedSlot*>) {
            assertion(type == aasm::SlotType::QWord, "Type must be qword for NamedSlot*");
            return 8;

        } else {
            static_assert(false, "Unsupported type in Slot::compute_size");
            std::unreachable();
        }
    }

    aasm::SlotType m_type;
    std::size_t m_size;
    std::variant<std::int64_t, std::string, const LIRNamedSlot*, std::vector<LIRSlot>> m_value;
};