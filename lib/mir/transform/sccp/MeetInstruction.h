#pragma once

#include <unordered_set>
#include <vector>

#include "SccpLattice.h"
#include "mir/instruction/InstructionVisitor.h"
#include "mir/instruction/Terminator.h"

namespace details {
    class MeetInstruction final: public Visitor {
    public:
        MeetInstruction(std::unordered_set<const BasicBlock*>& reachable_blocks, SccpLattice& states, std::vector<const BasicBlock*>& cfg_worklist, std::vector<const ValueInstruction*>& ssa_worklist) noexcept:
            m_reachable_blocks(reachable_blocks),
            m_states(states),
            m_cfg_worklist(cfg_worklist),
            m_ssa_worklist(ssa_worklist) {}

        void meet(const Instruction& inst) noexcept {
            inst.visit(*this);
        }

    private:
        void update(const ValueInstruction* inst, const LatticeValue& incoming) noexcept {
            if (m_states.merge_state(inst, incoming)) {
                m_ssa_worklist.push_back(inst);
            }
        }

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

        std::unordered_set<const BasicBlock*>& m_reachable_blocks;
        SccpLattice& m_states;
        std::vector<const BasicBlock*>& m_cfg_worklist;
        std::vector<const ValueInstruction*>& m_ssa_worklist;
    };
}
