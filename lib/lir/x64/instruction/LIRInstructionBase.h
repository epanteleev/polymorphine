#pragma once

#include <functional>
#include <iosfwd>
#include <memory>

#include "lir/x64/operand/LIROperand.h"
#include "LIRVisitor.h"

class LIRInstructionBase {
    static constexpr auto NO_ID = std::numeric_limits<std::size_t>::max();

public:
    explicit LIRInstructionBase(std::vector<LIROperand> &&uses): m_id(NO_ID),
                                                  m_owner(nullptr),
                                                  m_uses(std::move(uses)) {}

    virtual ~LIRInstructionBase() = default;

    [[nodiscard]]
    std::span<LIROperand const> inputs() const noexcept {
        return m_uses;
    }

    [[nodiscard]]
    const LIROperand& in(const std::size_t idx) const {
        return m_uses.at(idx);
    }

    virtual void visit(LIRVisitor& visitor) = 0;

    void print(std::ostream &os) const;

    [[nodiscard]]
    const LIRBlock* owner() const noexcept {
        assertion(m_owner != nullptr, "owner is null");
        return m_owner;
    }

    template<typename Fn>
    [[nodiscard]]
    bool isa(const Fn& matcher) const noexcept {
        return matcher(this);
    }

protected:
    friend class LIRBlock;

    void connect(std::size_t idx, const LIRBlock* owner) {
        m_id = idx;
        m_owner = owner;
    }

    [[nodiscard]]

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
    const LIRBlock* m_owner;
    std::vector<LIROperand> m_uses;
};

template<typename T>
using LIRInstBuilder = std::function<std::unique_ptr<T>(std::size_t, LIRBlock*)>;