#pragma once

#include "base/analysis/AnalysisPassManagerBase.h"
#include "lir/x64/analysis/liveness/LiveInfo.h"
#include "lir/x64/analysis/regalloc/LinearScanBase.h"
#include "lir/x64/analysis/regalloc/RegisterAllocation.h"
#include "lir/x64/instruction/LIRAdjustStack.h"
#include "lir/x64/module/LIRFuncData.h"

template<call_conv::CallConv CC>
class CallInfoInitialize final {
    CallInfoInitialize(const RegisterAllocation& reg_allocation,
                               const LivenessAnalysisInfo& liveness_info,
                               LIRFuncData& func_data) noexcept:
        m_reg_allocation(reg_allocation),
        m_liveness_info(liveness_info),
        m_func_data(func_data) {}

public:
    static CallInfoInitialize create(AnalysisPassManagerBase<LIRFuncData>* manager, LIRFuncData* data) {
        const auto reg_allocation = manager->analyze<LinearScanBase<CC>>(data);
        const auto liveness_info = manager->analyze<LivenessAnalysis>(data);
        return {*reg_allocation, *liveness_info, *data};
    }

    void run() {
        for (const auto& bb: m_func_data.basic_blocks()) {
            for (auto& inst: bb.instructions()) {
                const auto adjust_inst = dynamic_cast<LIRAdjustStack*>(&inst);
                if (adjust_inst == nullptr) {
                    continue;
                }

                initialize_adjust_stack(adjust_inst);
            }
        }
    }

private:
    void initialize_adjust_stack(LIRAdjustStack* adjust_stack) {
        switch (adjust_stack->adjust_kind()) {
            case LIRAdjustKind::UpStack: {
                const auto& live_in = m_liveness_info.live_in(adjust_stack->owner());
                initialize_data(adjust_stack, adjust_stack->owner()->pred(0), live_in);
                break;
            }
            case LIRAdjustKind::DownStack: {
                const auto& live_out = m_liveness_info.live_out(adjust_stack->owner());
                initialize_data(adjust_stack, adjust_stack->owner(), live_out);
                break;
            }
            default: die("Unsupported LIRAdjustKind: {}", static_cast<int>(adjust_stack->adjust_kind()));
        }
    }

    void initialize_data(LIRAdjustStack * adjust_inst, const LIRBlock* call_holder_bb, const LIRValSet& live_set) {
        const auto call = find_call_instruction(call_holder_bb);
        const auto no_return_val = call->defs().empty();
        for (const auto& lir_val: live_set) {
            const auto reg = m_reg_allocation[lir_val];
            const auto gp_reg = reg.as_gp_reg();
            if (!gp_reg.has_value()) {
                continue;
            }
            if (!no_return_val && gp_reg.value() == aasm::rax) {
                continue;
            }

            try_add_register(adjust_inst, gp_reg.value());
        }
        adjust_inst->increase_stack_size(m_reg_allocation.local_area_size());
    }

    void try_add_register(LIRAdjustStack* adjust_inst, aasm::GPReg gp_reg) noexcept {
        if (!std::ranges::contains(CC::GP_CALLER_SAVE_REGISTERS, gp_reg)) {
            // If the register is a caller-saved register, we need to add it to the adjust stack.
            return;
        }

        // Add the register to the adjust stack instruction.
        adjust_inst->add_reg(gp_reg);
    }

    static const LIRCall* find_call_instruction(const LIRBlock* bb) {
        const auto last = bb->last();
        const auto call = dynamic_cast<const LIRCall*>(last);
        assertion(call != nullptr, "Expected LIRCall instruction at the end of the basic block");
        return call;
    }

    const RegisterAllocation& m_reg_allocation;
    const LivenessAnalysisInfo& m_liveness_info;
    LIRFuncData& m_func_data;
};
