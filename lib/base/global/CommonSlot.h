#pragma once

#include <cstdint>
#include <string>
#include <variant>
#include <vector>

#include "Constant.h"
#include "ZeroInit.h"
#include "base/Constant.h"
#include "base/global/SlotType.h"
#include <cassert>


template<typename Derived, typename G>
class CommonSlot {
protected:
    template<typename T>
    explicit CommonSlot(T&& data, const std::size_t size) noexcept:
        m_size(size),
        m_value(std::forward<T>(data)) {
#ifndef NDEBUG
        const auto vis = [&]<typename U>(const U& v) {
            const auto actual = compute_size(v);
            assertion(actual == size, "should be v.size={}, size={}", actual, size);
        };
        std::visit(vis, m_value);
#endif
    }

public:
    template<typename U>
    explicit CommonSlot(U&& data) noexcept:
        m_size(compute_size(data)),
        m_value(std::forward<U>(data)) {}

    template<typename Fn>
    decltype(auto) visit(Fn&& fn) const {
        return std::visit(std::forward<Fn>(fn), m_value);
    }

    [[nodiscard]]
    std::size_t size() const noexcept {
        return m_size;
    }

protected:
    template<typename T>
    static std::size_t compute_size(const T& data) noexcept {
        if constexpr (std::is_same_v<T, std::string>) {
            return data.size();

        } else if constexpr (std::is_same_v<T, Constant>) {
            switch (data.type()) {
                case SlotType::Byte:  return cst::BYTE_SIZE;
                case SlotType::Word:  return cst::WORD_SIZE;
                case SlotType::DWord: return cst::DWORD_SIZE;
                case SlotType::QWord: return cst::QWORD_SIZE;
                default: std::unreachable();
            }

        } else if constexpr (std::is_same_v<T, ZeroInit>) {
            return data.length();

        } else if constexpr (std::is_same_v<T, std::vector<Derived>>) {
            std::size_t total_size{};
            for (const auto& slot: data) {
                total_size += slot.size();
            }
            return total_size;

        } else if constexpr (std::is_same_v<T, const G*>) {
            return cst::POINTER_SIZE;

        } else {
            static_assert(false, "Unsupported type in Slot::compute_size");
            std::unreachable();
        }
    }

    std::size_t m_size;
    std::variant<Constant, ZeroInit, std::string, const G*, std::vector<Derived>> m_value;
};
