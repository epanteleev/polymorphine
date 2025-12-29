#pragma once

#include "base/InstLocation.h"

class VerifierResult final {
    template<typename T>
    constexpr explicit VerifierResult(const InstLocation& loc, T&& args) noexcept:
        m_loc(loc),
        m_result(std::forward<T>(args)) {}

public:
    static VerifierResult wrong_type(const InstLocation& loc, const Type *a_type, const Type *b_type) noexcept {
        return VerifierResult(loc, WrongBinaryType(a_type, b_type));
    }

    static VerifierResult wrong_type(const InstLocation loc, const Type *a_type) noexcept {
        return VerifierResult(loc, WrongUnaryType(a_type));
    }

    template<typename Os>
    friend Os& operator<<(Os& os, const VerifierResult& res) noexcept {
        const auto vis = [&]<typename T>(const T& arg) noexcept {
            os << arg << " in " << res.m_loc;
        };

        std::visit(vis, res.m_result);
        return os;
    }

private:
    struct WrongBinaryType final {
        const Type *a_type;
        const Type *b_type;

        template<typename Os>
        friend Os& operator<<(Os& os, const WrongBinaryType& msg) noexcept {
            return os << "wrong type: a=" << *msg.a_type << ", b=" << *msg.b_type;
        }
    };

    struct WrongUnaryType final {
        const Type *a_type;

        template<typename Os>
        friend Os& operator<<(Os& os, const WrongUnaryType& msg) noexcept {
            return os << "wrong type: a=" << *msg.a_type;
        }
    };

    InstLocation m_loc;
    std::variant<WrongBinaryType, WrongUnaryType> m_result{};
};