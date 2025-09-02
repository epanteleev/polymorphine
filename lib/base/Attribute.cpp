#include "Attribute.h"

#include <ostream>
#include <ranges>

std::ostream& operator<<(std::ostream& os, const Attribute& attr) {
    switch (attr) {
        case Attribute::ByValue: return os << "!byval";
        default: return os;
    }
}

static std::array attributes = {
    Attribute::ByValue,
};

std::ostream& operator<<(std::ostream& os, const AttributeSet& attr) {
    os << '[';
    for (const auto idx: std::ranges::iota_view{0U, AttributeSet::BITS}) {
        if (!attr.test(idx)) {
            continue;
        }

        if (idx > 0) {
            os << ' ';
        }
        os << attributes[idx];
    }
    return os << ']';
}