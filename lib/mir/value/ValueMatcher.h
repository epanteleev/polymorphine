#pragma once

#include "Value.h"

namespace impls {
    template<typename T>
    bool value_inst(const Value& value) noexcept {
        if (!value.is<ValueInstruction*>()) {
            return false;
        }

        const auto value_instruction = value.get<ValueInstruction*>();
        return dynamic_cast<const T*>(value_instruction) != nullptr;
    }

    template<typename... Args>
    bool match_args(const std::span<const Value>& values, Args&& ...args) noexcept {
        std::size_t index{};
        const auto matcher = [&]<typename T>(T&& arg) {
            if (!arg(values[index])) {
                return false;
            }
            index++;
            return true;
        };

        return (matcher(std::forward<Args>(args)) && ...);
    }
}