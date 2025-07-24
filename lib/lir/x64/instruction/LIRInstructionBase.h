#pragma once

#include <functional>
#include <iosfwd>
#include <memory>

#include "lir/x64/operand/LIROperand.h"
#include "LIRVisitor.h"

class LIRInstructionBase {
public:
    LIRInstructionBase(const std::size_t id, LIRBlock *bb, std::vector<LIROperand> &&uses): m_id(id),
                                                  m_owner(bb),
                                                  m_uses(std::move(uses)) {}

    virtual ~LIRInstructionBase() = default;

    [[nodiscard]]
    std::span<LIROperand const> inputs() const noexcept {
        return m_uses;
    }

    [[nodiscard]]
    std::span<LIRVal const> defs() const noexcept {
        return m_defs;
    }

    [[nodiscard]]
    const LIRVal& def(const std::size_t idx) const {
        return m_defs.at(idx);
    }

    [[nodiscard]]
    const LIROperand& in(const std::size_t idx) const {
        return m_uses.at(idx);
    }

    virtual void visit(LIRVisitor& visitor) = 0;

    void print(std::ostream &os) const;

    [[nodiscard]]
    const LIRBlock* owner() const noexcept {
        return m_owner;
    }

    template<typename Fn>
    [[nodiscard]]
    bool isa(const Fn& matcher) const noexcept {
        return matcher(this);
    }

protected:
    void add_def(const LIRVal& def) {
        m_defs.push_back(def);
    }

    static std::vector<LIRVal> to_vregs_only(std::span<LIROperand const> inputs) {
        std::vector<LIRVal> vregs;
        vregs.reserve(inputs.size());
        for (const auto& in: inputs) {
            const auto vreg = LIRVal::try_from(in);
            assertion(vreg.has_value(), "invariant");
            vregs.push_back(vreg.value());
        }
        return vregs;
    }

    std::size_t m_id;
    LIRBlock* m_owner;
    std::vector<LIRVal> m_defs;
    std::vector<LIROperand> m_uses;
};

template<typename T>
using LIRInstBuilder = std::function<std::unique_ptr<T>(std::size_t, LIRBlock*)>;