#pragma once

#include <cstdint>
#include <iosfwd>

#include "base/global/CommonSlot.h"
#include "lir/x64/lir_frwd.h"

template<typename T>
concept SlotVariant = std::convertible_to<T, Constant> ||
    std::convertible_to<T, ZeroInit> ||
    std::convertible_to<T, std::string_view> ||
    std::convertible_to<T, std::vector<LIRSlot>> ||
    std::convertible_to<T, const LIRNamedSlot*>;

class LIRSlot final: public CommonSlot<LIRSlot, LIRNamedSlot> {
public:
    explicit LIRSlot(Constant&& data) noexcept:
        CommonSlot(data) {}

    explicit LIRSlot(ZeroInit&& data) noexcept:
        CommonSlot(data) {}

    explicit LIRSlot(const std::string& data) noexcept:
        CommonSlot(data) {}

    explicit LIRSlot(std::vector<LIRSlot>&& data) noexcept:
        CommonSlot(std::move(data)) {}

    explicit LIRSlot(const LIRNamedSlot* data) noexcept:
        CommonSlot(data) {}

    void print_description(std::ostream &os) const;
};