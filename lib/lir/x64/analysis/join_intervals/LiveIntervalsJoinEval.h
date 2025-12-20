#pragma once

#include "LiveIntervalsGroups.h"
#include "asm/x64/reg/AnyRegMap.h"
#include "base/analysis/AnalysisPass.h"
#include "base/analysis/AnalysisPassManagerBase.h"

#include "lir/x64/analysis/intervals/LiveIntervals.h"


class LiveIntervalsJoinEval final {
public:
    using result_type = LiveIntervalsGroups;
    using basic_block = LIRBlock;
    static constexpr auto analysis_kind = AnalysisType::LiveIntervalsGroups;

private:
    LiveIntervalsJoinEval(const LiveIntervals& intervals, const LIRFuncData& data) noexcept:
        m_intervals(intervals),
        m_data(data) {}

public:
    void run() {
        collect_fixed_regs();
        setup_fixed_reg_groups();
        setup_parallel_copy_groups();
        do_joining();
    }

    [[nodiscard]]
    std::unique_ptr<LiveIntervalsGroups> result() {
        return std::make_unique<LiveIntervalsGroups>(std::move(m_groups), std::move(m_group_mapping));
    }

    [[nodiscard]]
    static LiveIntervalsJoinEval create(AnalysisPassManagerBase<LIRFuncData> *cache, const LIRFuncData *data);

private:
    void collect_fixed_regs() {
        collect_argument_regs();
        collect_fixed_regs_for_instructions();
    }

    void collect_gp_argument(const LIRVal& arg, const GPVReg& vreg);
    void collect_xmm_argument(const LIRVal& arg, const XVReg& vreg);
    void collect_argument_regs();

    void collect_fixed_regs_for_instructions();
    void collect_fixed_regs_for_instruction(const LIRInstructionBase& inst);

    void setup_fixed_reg_groups();

    void setup_parallel_copy_groups();

    void do_joining();

    /** Adds inputs of the instruction to the worklist if they are not part of any group yet. */
    void add_to_worklist_if_not_in_group(const LIRInstructionBase* inst, std::vector<LIRVal>& worklist) const;

    /** Adds new member to the groups with its merged live interval. */
    void add_group(LiveInterval&& live_interval, std::vector<LIRVal>&& members, const std::optional<aasm::Reg> &fixed_register);

    /** Merges the live intervals of the given LIR values into a new live interval. */
    LiveInterval create_live_intervals(const std::span<LIRVal>& lir_values) const noexcept;

    const LiveIntervals& m_intervals;
    const LIRFuncData& m_data;

    aasm::RegMap<std::vector<LIRVal>> m_reg_to_lir_val{};
    std::deque<Group> m_groups;
    LIRValMap<LiveIntervalsGroups::group_iterator> m_group_mapping{};
};