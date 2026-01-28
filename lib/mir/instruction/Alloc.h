#pragma once

#include "ValueInstruction.h"
#include "mir/types/PointerType.h"
#include "mir/value/ValueMatcher.h"


class Alloc final: public ValueInstruction {
public:
    explicit Alloc(const NonTrivialType *ty) noexcept :
        ValueInstruction(PointerType::ptr(), {}),
        m_type(ty) {}

    void visit(Visitor &visitor) override { visitor.accept(this); }

    static std::unique_ptr<Alloc> alloc(const NonTrivialType* ty) {
        return std::make_unique<Alloc>(ty);
    }

    [[nodiscard]]
    const NonTrivialType* allocated_type() const noexcept { return m_type; }

    template <std::derived_from<Instruction> I>
    static const Alloc* cast(const I* instruction) noexcept {
        return dynamic_cast<const Alloc*>(instruction);
    }

private:
    const NonTrivialType *m_type;
};

namespace impl {
    inline bool alloc(const Instruction* inst) noexcept {
        return dynamic_cast<const Alloc*>(inst) != nullptr;
    }
}

consteval auto alloc() {
    return impl::alloc;
}

consteval auto alloc_v() noexcept {
    return impls::value_inst<Alloc>;
}