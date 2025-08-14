#include "LIRFunctionCodegen.h"
#include "LIRInstructionCodegen.h"

void LIRFunctionCodegen::setup_basic_block_labels() {
    for (const auto& bb: m_preorder) {
        if (bb == m_data.first()) {
            // Skip the first basic block, it does not need a label.
            continue;
        }

        const auto label = m_as.create_label();
        m_bb_labels.emplace(bb, label);
    }
}

const ClobberRegs & LIRFunctionCodegen::clobber_reg(const LIRInstructionBase *inst) const  {
    if (const auto it = m_reg_allocation.try_get_clobber_regs(inst); it.has_value()) {
        return *it.value();
    }

    static ClobberRegs empty_clobber_regs;
    return empty_clobber_regs;
}

void LIRFunctionCodegen::traverse_instructions() {
    for (const auto& bb: m_preorder) {
        if (bb != m_data.first()) {
            const auto label = m_bb_labels.at(bb);
            m_as.set_label(label);
        }

        for (auto& inst: bb->instructions()) {
            LIRInstructionCodegen codegen(clobber_reg(&inst), m_reg_allocation, m_symbol_tab, m_bb_labels, m_as);
            codegen.run(&inst);
        }
    }
}