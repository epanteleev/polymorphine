#pragma once

#include <iosfwd>
#include <utility>

#include "asm/asm_frwd.h"
#include "base/global/CommonSlot.h"


namespace aasm {
    class Slot final : public CommonSlot<Slot, Directive> {
    public:
        template<typename T>
        explicit Slot(T &&data, const std::size_t size) noexcept:
            CommonSlot(std::forward<T>(data), size) {}

        void print_description(std::ostream &os) const;
    };
}