#pragma once

#include <cstdint>
#include <iosfwd>
#include <string>
#include <utility>
#include <variant>
#include <vector>

#include "SlotType.h"
#include "asm/asm_frwd.h"


namespace aasm {
    class Slot final {
    public:
        template<typename T>
        explicit Slot(T&& data, const SlotType type, const std::size_t size) noexcept:
            m_type(type),
            m_size(size),
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
        SlotType m_type;
        std::size_t m_size;
        std::variant<std::int64_t, std::string, const Directive*, std::vector<Slot>> m_value;
    };
}