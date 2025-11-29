#pragma once

#include "VRegSelection.h"

#include "lir/x64/analysis/intervals/LiveIntervals.h"
#include "lir/x64/analysis/intervals/LiveIntervalsEval.h"
#include "lir/x64/analysis/join_intervals/LiveIntervalsGroups.h"
#include "lir/x64/asm/cc/CallConv.h"


class LinearScan final {
    explicit LinearScan(const LIRFuncData &obj_func_data, details::VRegSelection &&reg_set,
                            const LiveIntervals &intervals, const LiveIntervalsGroups &groups,
                            const Ordering<LIRBlock> &preorder, aasm::SymbolTable &symbol_tab,
                            const call_conv::CallConvProvider *call_conv) noexcept :
        m_obj_func_data(obj_func_data),
        m_intervals(intervals),
        m_groups(groups),
        m_preorder(preorder),
        m_reg_set(std::move(reg_set)),
        m_symbol_tab(symbol_tab),
        m_call_conv(call_conv) {}

public:
    void run();
    static LinearScan create(AnalysisPassManagerBase<LIRFuncData> *cache, const LIRFuncData *data, aasm::SymbolTable& symbol_tab, const call_conv::CallConvProvider* call_conv);

private:
    void allocate_fixed_registers();
    void setup_unhandled_intervals();
    void do_register_allocation();
    void finalize_prologue_epilogue() const;

    void erase_active(const LiveInterval& unhandled_interval);
    void erase_unactive(const LiveInterval& unhandled_interval);
    void activate_unhandled_fixed_regs(const LiveInterval& unhandled_interval);

    [[nodiscard]]
    const LiveInterval* get_real_interval(const IntervalEntry& entry) const;
    void select_virtual_reg(const LIRVal& lir_val, IntervalHint hint);

    void allocate_register(const LIRVal& lir_val, const GPVReg& reg);
    void allocate_register(const LIRVal &lir_val, const aasm::Reg &reg);
    void allocate_register(const LIRVal& lir_val, aasm::GPReg reg);
    void allocate_register(const LIRVal& lir_val, aasm::XmmReg reg);

    void allocate_temporal_register(LIRInstructionBase* inst) const noexcept;

    void do_stack_alloc(const LIRVal& lir_val);
    void instruction_ordering();

    const LIRFuncData& m_obj_func_data;
    const LiveIntervals& m_intervals;
    const LiveIntervalsGroups& m_groups;
    const Ordering<LIRBlock>& m_preorder;

    details::VRegSelection m_reg_set;
    aasm::SymbolTable& m_symbol_tab;
    const call_conv::CallConvProvider* m_call_conv;

    aasm::GPRegSet m_used_callee_saved_regs{};

    std::vector<IntervalEntry> m_unhandled_intervals{};
    std::vector<IntervalEntry> m_inactive_intervals{};
    std::vector<IntervalEntry> m_active_intervals{};

    std::vector<LIRInstructionBase*> m_instruction_ordering{};
};