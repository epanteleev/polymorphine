#pragma once

#include <optional>
#include <utility>

#include "mir/types/FlagType.h"
#include "mir/types/FloatingPointType.h"
#include "mir/types/IntegerType.h"
#include "mir/types/PrimitiveType.h"

class VConstant final {
public:
    constexpr VConstant(const double value, const FloatingPointType *type) noexcept:
        m_fp(value),
        m_type(type) {}

    constexpr VConstant(const std::int64_t value, const IntegerType * type) noexcept:
        m_i64(value),
        m_type(type) {}

    constexpr explicit VConstant(const bool value) noexcept:
        m_bool(value),
        m_type(FlagType::flag()) {}

    [[nodiscard]]
    const Type* type() const noexcept {
        return m_type;
    }

    template <typename Visitor>
    decltype(auto) visit(Visitor&& visitor) const {
        if (FloatingPointType::cast(m_type) != nullptr) {
            return visitor(m_fp);
        }
        if (IntegerType::cast(m_type) != nullptr) {
            return visitor(m_fp);
        }
        if (FlagType::cast(m_type) != nullptr) {
            return visitor(m_fp);
        }
        std::unreachable();
    }

    [[nodiscard]]
    static std::optional<VConstant> sum(const VConstant& lhs, const VConstant& rhs) noexcept;

    [[nodiscard]]
    static std::optional<VConstant> sub(const VConstant& lhs, const VConstant& rhs) noexcept;

    [[nodiscard]]
    static std::optional<VConstant> mul(const VConstant& lhs, const VConstant& rhs) noexcept;

    [[nodiscard]]
    static std::optional<VConstant> div(const VConstant& lhs, const VConstant& rhs) noexcept;

    [[nodiscard]]
    static std::optional<VConstant> mod(const VConstant& lhs, const VConstant& rhs) noexcept;

    [[nodiscard]]
    static std::optional<VConstant> lt(const VConstant& lhs, const VConstant& rhs) noexcept;

    [[nodiscard]]
    static std::optional<VConstant> le(const VConstant& lhs, const VConstant& rhs) noexcept;

    [[nodiscard]]
    static std::optional<VConstant> eq(const VConstant& lhs, const VConstant& rhs) noexcept;

    [[nodiscard]]
    static std::optional<VConstant> ne(const VConstant& lhs, const VConstant& rhs) noexcept;

    [[nodiscard]]
    static std::optional<VConstant> gt(const VConstant& lhs, const VConstant& rhs) noexcept;

    [[nodiscard]]
    static std::optional<VConstant> ge(const VConstant& lhs, const VConstant& rhs) noexcept;

    [[nodiscard]]
    static std::optional<VConstant> try_from(const Value& value) noexcept;

private:
    template<typename Fn>
    static std::optional<VConstant> compare(const VConstant& lhs, const VConstant& rhs, Fn&& cmp) noexcept {
        if (lhs.m_type != rhs.m_type) {
            return std::nullopt;
        }

        if (FloatingPointType::cast(lhs.m_type) != nullptr) {
            return VConstant(cmp(lhs.m_fp, rhs.m_fp));
        }

        if (IntegerType::cast(lhs.m_type) != nullptr) {
            return VConstant(cmp(lhs.m_i64, rhs.m_i64));
        }

        return std::nullopt;
    }

    union {
        double m_fp;
        bool m_bool;
        std::int64_t m_i64;
    };
    const Type* m_type;
};