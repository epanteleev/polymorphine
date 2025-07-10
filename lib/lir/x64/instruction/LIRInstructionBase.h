#pragma once

#include <functional>
#include <iosfwd>
#include <memory>

#include "lir/x64/operand/LIROperand.h"
#include "LIRVisitor.h"

class LIRInstructionBase {
public:
    LIRInstructionBase(const std::size_t id, MachBlock *bb, std::vector<LIROperand> &&uses,
                       std::vector<VReg> &&defs): m_id(id),
                                                  m_owner(bb),
                                                  m_defs(defs),
                                                  m_uses(std::move(uses)) {}

    virtual ~LIRInstructionBase() = default;

    [[nodiscard]]
    std::span<LIROperand const> inputs() const noexcept {
        return m_uses;
    }

    [[nodiscard]]
    std::span<VReg const> defs() const noexcept {
        return m_defs;
    }

    [[nodiscard]]
    const VReg& def(const std::size_t idx) const {
        return m_defs.at(idx);
    }

    [[nodiscard]]
    const LIROperand& in(const std::size_t idx) const {
        return m_uses.at(idx);
    }

    virtual void visit(LIRVisitor& visitor) = 0;

    void print(std::ostream &os) const;

    [[nodiscard]]
    const MachBlock* owner() const noexcept {
        return m_owner;
    }

    template<typename Fn>
    [[nodiscard]]
    bool isa(const Fn& matcher) const noexcept {
        return matcher(this);
    }

protected:
    void add_def(const VReg& def) {
        m_defs.push_back(def);
    }

    static std::vector<VReg> to_vregs_only(std::span<LIROperand const> inputs) {
        std::vector<VReg> vregs;
        vregs.reserve(inputs.size());
        for (const auto& in: inputs) {
            const auto vreg = VReg::try_from(in);
            assertion(vreg.has_value(), "invariant");
            vregs.push_back(vreg.value());
        }
        return vregs;
    }

    std::size_t m_id;
    MachBlock* m_owner;
    std::vector<VReg> m_defs;
    std::vector<LIROperand> m_uses;
};

template<typename T>
using LIRInstBuilder = std::function<std::unique_ptr<T>(std::size_t, MachBlock*)>;