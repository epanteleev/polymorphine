#pragma once

#include <unordered_set>

#include "SccpLattice.h"
#include "mir/instruction/InstructionVisitor.h"
#include "mir/instruction/Terminator.h"

namespace details {
    class MeetInstruction final: public Visitor {
    public:
        explicit MeetInstruction(std::unordered_set<const BasicBlock*>& executable_blocks, SccpLattice& states) noexcept:
            m_reachable_blocks(executable_blocks),
            m_states(states) {}

        [[nodiscard]]
        bool meet(const Instruction &inst) noexcept;

    private:
        void terminator(const Instruction *inst) noexcept;

        void accept(Binary *inst) override;

        void accept(Unary *inst) override {}

        void accept(Branch *branch) override;

        void accept(CondBranch *cond_branch) override;

        void accept(Call *inst) override;

        void accept(TupleCall *inst) override {}

        void accept(Return *inst) override;

        void accept(ReturnValue *inst) override;

        void accept(Switch *inst) override;

        void accept(VCall *call) override;

        void accept(IVCall *call) override;

        void accept(Phi *inst) override;

        void accept(Store *store) override {

        }

        void accept(Alloc *alloc) override {

        }

        void accept(IcmpInstruction *icmp) override;

        void accept(FcmpInstruction *fcmp) override {

        }

        void accept(GetElementPtr *gep) override {

        }

        void accept(GetFieldPtr *gfp) override {

        }

        void accept(Select *select) override;

        void accept(IntDiv *div) override {

        }

        void accept(Projection *proj) override {

        }

        bool m_changed{};
        std::unordered_set<const BasicBlock*>& m_reachable_blocks;
        SccpLattice& m_states;
    };
}