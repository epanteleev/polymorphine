#pragma once

#include "base/InstLocation.h"

class VerifierResult final {
    template<typename T>
    constexpr explicit VerifierResult(const FunctionPrototype* prototype, const InstLocation& loc, T&& args) noexcept:
        m_prototype(prototype),
        m_loc(loc),
        m_result(std::forward<T>(args)) {}

public:
    static VerifierResult wrong_type(const FunctionPrototype* prototype, const InstLocation& loc, const Type *a_type, const Type *b_type) noexcept {
        return VerifierResult(prototype, loc, WrongBinaryType(a_type, b_type));
    }

    static VerifierResult wrong_type(const FunctionPrototype* prototype, const InstLocation loc, const Type *a_type) noexcept {
        return VerifierResult(prototype, loc, WrongUnaryType(a_type));
    }

    static VerifierResult invalid_cfg(const FunctionPrototype* prototype, const InstLocation& loc, const std::size_t bb_id, const std::size_t pred_id) noexcept {
        return VerifierResult(prototype, loc, InvalidControlFlow(bb_id, pred_id));
    }

    static VerifierResult invalid_terminator(const FunctionPrototype* prototype, const InstLocation& loc, const std::size_t bb_id) noexcept {
        return VerifierResult(prototype, loc, InvalidTerminator(bb_id));
    }

    static VerifierResult invalid_du(const FunctionPrototype* prototype, const InstLocation& loc) noexcept {
        return VerifierResult(prototype, loc, InvalidDUChain());
    }

    template<typename Os>
    friend Os& operator<<(Os& os, const VerifierResult& res) noexcept {
        const auto vis = [&]<typename T>(const T& arg) noexcept {
            os << arg << " in " << *res.m_prototype << " " << res.m_loc;
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

    struct InvalidControlFlow final {
        const std::size_t bb_id;
        const std::size_t pred_id;

        template<typename Os>
        friend Os& operator<<(Os& os, const InvalidControlFlow& msg) noexcept {
            return os << "CFG error: basic block '" << msg.bb_id << "' is not a successor of " << msg.pred_id;
        }
    };

    struct InvalidTerminator final {
        const std::size_t bb_id;

        template<typename Os>
        friend Os& operator<<(Os& os, const InvalidTerminator& msg) noexcept {
            return os << "CFG error: basic block '" << msg.bb_id << "' ends with a return instruction but has successors";
        }
    };

    struct InvalidDUChain final {
        template<typename Os>
        friend Os& operator<<(Os& os, const InvalidDUChain&) noexcept {
            return os << "Def-Use chain error in function";
        }
    };

    const FunctionPrototype* m_prototype;
    InstLocation m_loc;
    std::variant<WrongBinaryType,
        WrongUnaryType,
        InvalidControlFlow,
        InvalidTerminator,
        InvalidDUChain> m_result{};
};