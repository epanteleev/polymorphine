#include "Constant.h"
#include "mir/value/Value.h"
#include "utility/ArithmeticUtils.h"

std::optional<Constant> Constant::sum(const Constant &lhs, const Constant &rhs) noexcept {
    if (lhs.m_type != rhs.m_type) {
        return std::nullopt;
    }

    if (const auto *fp_type = FloatingPointType::cast(lhs.m_type)) {
        return Constant(lhs.m_fp + rhs.m_fp, fp_type);
    }

    if (const auto *int_type = IntegerType::cast(lhs.m_type)) {
        return Constant(add_overflow(lhs.m_i64, rhs.m_i64), int_type);
    }

    return std::nullopt;
}

std::optional<Constant> Constant::sub(const Constant &lhs, const Constant &rhs) noexcept {
    if (lhs.m_type != rhs.m_type) {
        return std::nullopt;
    }

    if (const auto *fp_type = FloatingPointType::cast(lhs.m_type)) {
        return Constant(lhs.m_fp - rhs.m_fp, fp_type);
    }

    if (const auto *int_type = IntegerType::cast(lhs.m_type)) {
        return Constant(add_overflow(lhs.m_i64, rhs.m_i64), int_type);
    }

    return std::nullopt;
}

std::optional<Constant> Constant::mul(const Constant &lhs, const Constant &rhs) noexcept {
    if (lhs.m_type != rhs.m_type) {
        return std::nullopt;
    }

    if (const auto *fp_type = FloatingPointType::cast(lhs.m_type)) {
        return Constant(lhs.m_fp * rhs.m_fp, fp_type);
    }

    if (const auto *int_type = IntegerType::cast(lhs.m_type)) {
        return Constant(mul_overflow(lhs.m_i64, rhs.m_i64), int_type);
    }

    return std::nullopt;
}

std::optional<Constant> Constant::div(const Constant &lhs, const Constant &rhs) noexcept {
    if (lhs.m_type != rhs.m_type) {
        return std::nullopt;
    }

    if (const auto *fp_type = FloatingPointType::cast(lhs.m_type)) {
        return Constant(lhs.m_fp / rhs.m_fp, fp_type);
    }

    if (const auto *int_type = IntegerType::cast(lhs.m_type)) {
        if (rhs.m_i64 == 0) { // Undefined behaviour
            return Constant(0, int_type);
        }

        return Constant(lhs.m_i64 / rhs.m_i64, int_type);
    }

    return std::nullopt;
}

std::optional<Constant> Constant::mod(const Constant &lhs, const Constant &rhs) noexcept {
    if (lhs.m_type != rhs.m_type) {
        return std::nullopt;
    }

    if (const auto *int_type = IntegerType::cast(lhs.m_type)) {
        if (rhs.m_i64 == 0) { // Undefined behaviour
            return Constant(0, int_type);
        }

        return Constant(lhs.m_i64 % rhs.m_i64, int_type);
    }

    return std::nullopt;
}

std::optional<Constant> Constant::try_from(const Value &value) noexcept {
    if (value.is<double>()) {
        const auto *type = FloatingPointType::cast(value.type());
        if (type == nullptr) {
            return std::nullopt;
        }
        return Constant(value.get<double>(), type);
    }

    if (value.is<std::int64_t>()) {
        const auto *type = IntegerType::cast(value.type());
        if (type == nullptr) {
            return std::nullopt;
        }
        return Constant(value.get<std::int64_t>(), type);
    }

    return std::nullopt;
}
