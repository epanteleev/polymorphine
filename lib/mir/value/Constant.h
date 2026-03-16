#pragma once

#include <optional>

#include "mir/types/FloatingPointType.h"
#include "mir/types/IntegerType.h"
#include "mir/types/PrimitiveType.h"

class Constant final {
public:
    constexpr Constant(const double value, const FloatingPointType *type) noexcept:
        m_fp(value),
        m_type(type) {}

    constexpr Constant(const std::int64_t value, const IntegerType * type) noexcept:
        m_i64(value),
        m_type(type) {}

    [[nodiscard]]
    static std::optional<Constant> sum(const Constant& lhs, const Constant& rhs) noexcept;

    [[nodiscard]]
    static std::optional<Constant> sub(const Constant& lhs, const Constant& rhs) noexcept;

    [[nodiscard]]
    static std::optional<Constant> mul(const Constant& lhs, const Constant& rhs) noexcept;

    [[nodiscard]]
    static std::optional<Constant> div(const Constant& lhs, const Constant& rhs) noexcept;

    [[nodiscard]]
    static std::optional<Constant> mod(const Constant& lhs, const Constant& rhs) noexcept;

    [[nodiscard]]
    static std::optional<Constant> try_from(const Value& value) noexcept;

private:
    union {
        double m_fp;
        std::int64_t m_i64;
    };
    const PrimitiveType* m_type;
};