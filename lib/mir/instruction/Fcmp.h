#pragma once

#include <memory>
#include "Compare.h"

enum class FcmpPredicate: std::uint8_t {
    // Ordered
    Oeq,
    One,
    Olt,
    Ole,
    Ogt,
    Oge,
    Ord,
    // Unordered
    Ueq,
    Une,
    Ult,
    Ule,
    Ugt,
    Uge,
    Uno,
};

inline std::string_view to_string(const FcmpPredicate pred) noexcept {
    switch (pred) {
        case FcmpPredicate::Oeq: return "oeq";
        case FcmpPredicate::One: return "one";
        case FcmpPredicate::Olt: return "olt";
        case FcmpPredicate::Ole: return "ole";
        case FcmpPredicate::Ogt: return "ogt";
        case FcmpPredicate::Oge: return "oge";
        case FcmpPredicate::Ord: return "ord";
        case FcmpPredicate::Ueq: return "ueq";
        case FcmpPredicate::Une: return "une";
        case FcmpPredicate::Ult: return "ult";
        case FcmpPredicate::Ule: return "ule";
        case FcmpPredicate::Ugt: return "ugt";
        case FcmpPredicate::Uge: return "uge";
        case FcmpPredicate::Uno: return "uno";
        default: std::unreachable();
    }
}

class FcmpInstruction final: public Compare {
public:
    explicit FcmpInstruction(const FcmpPredicate pred, const Value& lhs, const Value& rhs) noexcept:
        Compare(lhs, rhs),
        m_pred(pred) {}

    void visit(Visitor &visitor) override { visitor.accept(this); }

    [[nodiscard]]
    FcmpPredicate predicate() const noexcept {
        return m_pred;
    }

    [[nodiscard]]
    static std::unique_ptr<FcmpInstruction> fcmp(const FcmpPredicate pred, const Value& lhs, const Value& rhs) {
        return std::make_unique<FcmpInstruction>(pred, lhs, rhs);
    }

private:
    const FcmpPredicate m_pred;
};