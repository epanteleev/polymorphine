#include "Verifier.h"
#include "mir/instruction/Instruction.h"
#include "mir/instruction/Binary.h"

#include <ranges>

class InstructionVerifier final: public Visitor {
public:
    [[nodiscard]]
    static std::optional<VerifierResult> apply(const Instruction* inst) {
        InstructionVerifier instVer;
        const_cast<Instruction*>(inst)->visit(instVer);
        return instVer.m_correct;
    }

private:
    friend class Visitor;

    void accept(Binary *inst) override {
        const auto a_type = inst->lhs().type();
        const auto b_type = inst->rhs().type();
        if (dynamic_cast<const ArithmeticType*>(a_type) == nullptr) {
            m_correct.emplace(VerifierResult::wrong_type(inst->location(), a_type, b_type));
            return;
        }
        if (a_type != b_type) {
            m_correct.emplace(VerifierResult::wrong_type(inst->location(), a_type, b_type));
        }
    }

    void accept(Unary *inst) override {
    }

    void accept(Branch *branch) override {
    }

    void accept(CondBranch *cond_branch) override {
    }

    void accept(Call *inst) override {
    }

    void accept(TupleCall *inst) override {
    }

    void accept(Return *inst) override {
    }

    void accept(ReturnValue *inst) override {
    }

    void accept(Switch *inst) override {
    }

    void accept(VCall *call) override {
    }

    void accept(IVCall *call) override {
    }

    void accept(Phi *inst) override {
    }

    void accept(Store *store) override {
    }

    void accept(Alloc *alloc) override {
    }

    void accept(IcmpInstruction *icmp) override {
    }

    void accept(FcmpInstruction *fcmp) override {
    }

    void accept(GetElementPtr *gep) override {
    }

    void accept(GetFieldPtr *gfp) override {
    }

    void accept(Select *select) override {
    }

    void accept(IntDiv *div) override {
    }

    void accept(Projection *proj) override {
    }

    std::optional<VerifierResult> m_correct{};
};

std::optional<VerifierResult> Verifier::apply(const Module &module) {
    for (const auto& fn: std::ranges::views::values(module.functions())) {
        for (const auto& bb: fn.basic_blocks()) {
            for (const auto& inst: bb.instructions()) {
                if (auto res = InstructionVerifier::apply(&inst); res.has_value()) {
                    return res;
                }
            }
        }
    }

    return std::nullopt;
}