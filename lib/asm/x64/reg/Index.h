#pragma once
#include <concepts>
#include <cstddef>
#include <utility>

#include "GPReg.h"
#include "Reg.h"
#include "XmmReg.h"

namespace aasm::details {
    template<typename R>
    static constexpr R from_index(const std::size_t idx) noexcept {
        if constexpr (std::same_as<R, XmmReg>) {
            return R(idx+xmm0.code());
        } else if constexpr (std::same_as<R, GPReg>) {
            return R(idx);

        } else if constexpr (std::same_as<R, Reg>) {
            if (idx < GPReg::NUMBER_OF_REGISTERS) {
                return R(GPReg(idx));
            }
            return R(XmmReg(idx));

        } else {
            static_assert(false, "unexpected type");
            std::unreachable();
        }
    }

    template<typename R>
    static constexpr std::size_t to_index(const R& reg) noexcept {
        if constexpr (std::same_as<R, XmmReg>) {
            return reg.code()-xmm0.code();
        } else if constexpr (std::same_as<R, GPReg> || std::same_as<R, Reg>) {
            return reg.code();
        } else {
            static_assert(false, "unexpected type");
            std::unreachable();
        }
    }
}