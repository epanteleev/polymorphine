#include "VConstant.h"
#include "mir/value/Value.h"
#include "utility/ArithmeticUtils.h"

std::optional<VConstant> VConstant::sum(const VConstant &lhs, const VConstant &rhs) noexcept {
    if (lhs.m_type != rhs.m_type) {
        return std::nullopt;
    }

    if (const auto *fp_type = FloatingPointType::cast(lhs.m_type)) {
        return VConstant(lhs.m_fp + rhs.m_fp, fp_type);
    }

    if (const auto *int_type = IntegerType::cast(lhs.m_type)) {
        return VConstant(add_overflow(lhs.m_i64, rhs.m_i64), int_type);
    }

    return std::nullopt;
}

std::optional<VConstant> VConstant::sub(const VConstant &lhs, const VConstant &rhs) noexcept {
    if (lhs.m_type != rhs.m_type) {
        return std::nullopt;
    }

    if (const auto *fp_type = FloatingPointType::cast(lhs.m_type)) {
        return VConstant(lhs.m_fp - rhs.m_fp, fp_type);
    }

    if (const auto *int_type = IntegerType::cast(lhs.m_type)) {
        return VConstant(add_overflow(lhs.m_i64, rhs.m_i64), int_type);
    }

    return std::nullopt;
}

std::optional<VConstant> VConstant::mul(const VConstant &lhs, const VConstant &rhs) noexcept {
    if (lhs.m_type != rhs.m_type) {
        return std::nullopt;
    }

    if (const auto *fp_type = FloatingPointType::cast(lhs.m_type)) {
        return VConstant(lhs.m_fp * rhs.m_fp, fp_type);
    }

    if (const auto *int_type = IntegerType::cast(lhs.m_type)) {
        return VConstant(mul_overflow(lhs.m_i64, rhs.m_i64), int_type);
    }

    return std::nullopt;
}

std::optional<VConstant> VConstant::div(const VConstant &lhs, const VConstant &rhs) noexcept {
    if (lhs.m_type != rhs.m_type) {
        return std::nullopt;
    }

    if (const auto *fp_type = FloatingPointType::cast(lhs.m_type)) {
        return VConstant(lhs.m_fp / rhs.m_fp, fp_type);
    }

    if (const auto *int_type = IntegerType::cast(lhs.m_type)) {
        if (rhs.m_i64 == 0) { // Undefined behaviour
            return VConstant(0, int_type);
        }

        return VConstant(lhs.m_i64 / rhs.m_i64, int_type);
    }

    return std::nullopt;
}

std::optional<VConstant> VConstant::mod(const VConstant &lhs, const VConstant &rhs) noexcept {
    if (lhs.m_type != rhs.m_type) {
        return std::nullopt;
    }

    if (const auto *int_type = IntegerType::cast(lhs.m_type)) {
        if (rhs.m_i64 == 0) { // Undefined behaviour
            return VConstant(0, int_type);
        }

        return VConstant(lhs.m_i64 % rhs.m_i64, int_type);
    }

    return std::nullopt;
}

std::optional<VConstant> VConstant::lt(const VConstant &lhs, const VConstant &rhs) noexcept {
    const auto cmp = []<typename T>(const T &l, const T &r) {
        return l < r;
    };

    return compare(lhs, rhs, cmp);
}

std::optional<VConstant> VConstant::le(const VConstant &lhs, const VConstant &rhs) noexcept {
    const auto cmp = []<typename T>(const T &l, const T &r) {
        return l <= r;
    };

    return compare(lhs, rhs, cmp);
}

std::optional<VConstant> VConstant::eq(const VConstant &lhs, const VConstant &rhs) noexcept {
    const auto cmp = []<typename T>(const T &l, const T &r) {
        return l == r;
    };

    return compare(lhs, rhs, cmp);
}

std::optional<VConstant> VConstant::ne(const VConstant &lhs, const VConstant &rhs) noexcept {
    const auto cmp = []<typename T>(const T &l, const T &r) {
        return l != r;
    };

    return compare(lhs, rhs, cmp);
}

std::optional<VConstant> VConstant::gt(const VConstant &lhs, const VConstant &rhs) noexcept {
    const auto cmp = []<typename T>(const T &l, const T &r) {
        return l > r;
    };

    return compare(lhs, rhs, cmp);
}

std::optional<VConstant> VConstant::ge(const VConstant &lhs, const VConstant &rhs) noexcept {
    const auto cmp = []<typename T>(const T &l, const T &r) {
        return l >= r;
    };

    return compare(lhs, rhs, cmp);
}

std::optional<VConstant> VConstant::try_from(const Value &value) noexcept {
    if (value.is<double>()) {
        const auto *type = FloatingPointType::cast(value.type());
        return VConstant(value.get<double>(), type);
    }

    if (value.is<std::int64_t>()) {
        const auto *type = IntegerType::cast(value.type());
        return VConstant(value.get<std::int64_t>(), type);
    }

    if (value.is<bool>()) {
        return VConstant(value.get<bool>());
    }

    return std::nullopt;
}
