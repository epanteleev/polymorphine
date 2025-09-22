#pragma once

#include "AllocTemporalRegs.h"
#include "VRegSelection.h"
#include "asm/x64/reg/RegSet.h"

#include "lir/x64/analysis/intervals/LiveIntervals.h"
#include "lir/x64/analysis/intervals/LiveIntervalsEval.h"
#include "lir/x64/analysis/join_intervals/LiveIntervalsJoinEval.h"
#include "lir/x64/operand/OperandMatcher.h"
#include "lir/x64/asm/cc/CallConv.h"
#include "utility/StdExtensions.h"


class LinearScanBase final {
    explicit LinearScanBase(const LIRFuncData &obj_func_data, details::VRegSelection&& reg_set, const LiveIntervals& intervals, const LiveIntervalsGroups& groups, const Ordering<LIRBlock>& preorder, const call_conv::CallConvProvider* call_conv) noexcept:
        m_obj_func_data(obj_func_data),
        m_intervals(intervals),
        m_groups(groups),
        m_preorder(preorder),
        m_reg_set(std::move(reg_set)),
        m_call_conv(call_conv) {}

public:
    void run() {
        allocate_fixed_registers();
        instruction_ordering();
        setup_unhandled_intervals();
        do_register_allocation();
        finalize_prologue_epilogue();
    }

    static LinearScanBase create(AnalysisPassManagerBase<LIRFuncData> *cache, const LIRFuncData *data, const call_conv::CallConvProvider* call_conv) {
        const auto intervals = cache->analyze<LiveIntervalsEval>(data);
        const auto joins = cache->analyze<LiveIntervalsJoinEval>(data);
        const auto preorder = cache->analyze<PreorderTraverseBase<LIRFuncData>>(data);
        auto vreg_selection = details::VRegSelection::create(call_conv, collect_used_callee_saved_regs(data->args()));
        return LinearScanBase(*data, std::move(vreg_selection), *intervals, *joins, *preorder, call_conv);
    }

private:
    void allocate_fixed_registers() {
        for (const auto& group: m_groups) {
            const auto fixed_reg = group.fixed_register();
            if (!fixed_reg.has_value()) {
                continue;
            }

            for (const auto& lir: group.m_values) {
                allocate_register(lir, fixed_reg.value());
            }
        }
    }

    void setup_unhandled_intervals() {
        m_unhandled_intervals.reserve(m_intervals.intervals().size());
        m_active_intervals.reserve(m_intervals.intervals().size());

        for (auto& [lir_val, interval]: m_intervals.intervals()) {
            if (lir_val.arg().has_value()) {
                m_active_intervals.emplace_back(&interval, lir_val);
            } else {
                m_unhandled_intervals.emplace_back(&interval, lir_val);
            }
        }

        const auto pred = [](const IntervalEntry& lhs, const IntervalEntry& rhs) {
            return lhs.interval->start() > rhs.interval->start();
        };

        std::ranges::sort(m_unhandled_intervals, pred);
    }

    void do_register_allocation() {
        std::int64_t range_begin{};
        while (!m_unhandled_intervals.empty()) {
            auto [unhandled_interval, lir_val] = m_unhandled_intervals.back();
            m_unhandled_intervals.pop_back();

            if (lir_val.isa(gen_v())) {
                do_stack_alloc(lir_val);
                continue;
            }
            for (std::int64_t i = range_begin; i < unhandled_interval->start()-1; ++i) {
                allocate_temporal_register(m_instruction_ordering[i]);
            }
            range_begin = unhandled_interval->start();

            const auto active_eraser = [&](const IntervalEntry& entry) {
                const auto real_interval = get_real_interval(entry);
                const auto reg = entry.lir_val.assigned_reg().to_gp_op().value();
                if (real_interval->start() > unhandled_interval->finish()) {
                    if (const auto reg_opt = reg.as_gp_reg(); reg_opt.has_value()) {
                        m_reg_set.push(reg_opt.value());
                    }
                    return true;
                }

                if (real_interval->intersects(*unhandled_interval)) {
                    return false;
                }
                m_inactive_intervals.emplace_back(entry);
                if (const auto reg_opt = reg.as_gp_reg(); reg_opt.has_value()) {
                    m_reg_set.push(reg_opt.value());
                }

                return true;
            };

            remove_if_fast(m_active_intervals, active_eraser);

            const auto unactive_eraser = [&](const IntervalEntry& entry) {
                const auto real_interval = get_real_interval(entry);
                if (real_interval->start() > unhandled_interval->finish()) {
                    return true;
                }

                if (!real_interval->intersects(*unhandled_interval)) {
                    return false;
                }
                // This interval is still active, we need to keep it.
                m_active_intervals.emplace_back(entry);
                const auto reg = entry.lir_val.assigned_reg().to_gp_op().value();
                if (const auto reg_opt = reg.as_gp_reg(); reg_opt.has_value()) {
                    m_reg_set.remove(reg_opt.value());
                }

                return true;
            };

            remove_if_fast(m_inactive_intervals, unactive_eraser);

            for (const auto& unhandled: std::ranges::reverse_view(m_unhandled_intervals)) {
                const auto group = m_groups.try_get_group(unhandled.lir_val);
                if (!group.has_value()) {
                    // No group for this vreg, we can skip it.
                    continue;
                }

                const auto real_interval = &group.value()->m_interval;
                if (real_interval->start() > unhandled_interval->finish()) {
                    // No need to check further, the intervals are sorted.
                    break;
                }

                const auto fixed_reg_groups = group.value()->fixed_register();
                if (!fixed_reg_groups.has_value()) {
                    // This group does not have a fixed register, we can skip it too.
                    continue;
                }
                const auto fixed_gp_reg = fixed_reg_groups.value().as_gp_reg();
                if (!fixed_gp_reg.has_value()) {
                    continue;
                }

                if (!real_interval->intersects(*unhandled_interval)) {
                    // This interval does not intersect with the unhandled interval, skip it.
                    continue;
                }

                m_active_intervals.emplace_back(unhandled);
                m_reg_set.remove(fixed_gp_reg.value());
            }

            select_virtual_reg(lir_val, unhandled_interval->hint());
            m_active_intervals.emplace_back(unhandled_interval, lir_val);

            // Allocate temporals for current instruction.
            allocate_temporal_register(m_instruction_ordering[range_begin-1]);
        }

        // Process the remaining instructions.
        for (std::int64_t i = range_begin; i < static_cast<std::int64_t>(m_instruction_ordering.size()); ++i) {
            allocate_temporal_register(m_instruction_ordering[i]);
        }
    }

    void finalize_prologue_epilogue() const {
        const auto prologue = m_obj_func_data.prologue();
        const auto epilogue = m_obj_func_data.epilogue();
        for (const auto reg: m_used_callee_saved_regs) {
            epilogue->add_reg(reg);
            prologue->add_reg(reg);
        }
        epilogue->increase_local_area_size(m_reg_set.local_area_size());
        prologue->increase_local_area_size(m_reg_set.local_area_size());
    }

    [[nodiscard]]
    const LiveInterval* get_real_interval(const IntervalEntry& entry) const {
        if (const auto it = m_groups.try_get_group(entry.lir_val); it.has_value()) {
            return &it.value()->interval();
        }

        return entry.interval;
    }

    void select_virtual_reg(const LIRVal& lir_val, const IntervalHint hint) {
        if (const auto vreg = lir_val.assigned_reg().to_gp_op(); vreg.has_value()) {
            return;
        }

        if (const auto group = m_groups.try_get_group(lir_val); group.has_value()) {
            assertion(!group.value()->fixed_register().has_value(), "Group with fixed register should not be allocated here");
            const auto reg = m_reg_set.top(group.value()->hint());
            for (const auto& group_vreg: group.value()->m_values) {
                allocate_register(group_vreg, reg);
            }
            return;
        }

        const auto reg = m_reg_set.top(hint);
        allocate_register(lir_val, reg);
    }

    void allocate_register(const LIRVal& lir_val, const GPVReg& reg) {
        if (const auto gp_reg= reg.as_gp_reg(); gp_reg.has_value()) {
            allocate_register(lir_val, gp_reg.value());
            return;
        }

        if (const auto vreg = lir_val.assigned_reg().to_gp_op(); vreg.has_value()) {
            return;
        }

        lir_val.assign_reg(reg);
    }

    void allocate_register(const LIRVal& lir_val, const aasm::GPReg reg) {
        if (const auto vreg = lir_val.assigned_reg().to_gp_op(); vreg.has_value()) {
            return;
        }
        lir_val.assign_reg(reg);

        if (!std::ranges::contains(m_call_conv->GP_CALLEE_SAVE_REGISTERS(), reg)) {
            // This is a caller-save register, we can skip it.
            return;
        }

        if (std::ranges::contains(m_used_callee_saved_regs, reg)) {
            return; // Already added
        }

        m_used_callee_saved_regs.emplace(reg);
    }

    void allocate_temporal_register(LIRInstructionBase* inst) noexcept {
        if (!inst->temporal_regs().empty()) {
            return;
        }

        switch (const auto temp_num = details::AllocTemporalRegs::allocate(inst)) {
            case 0: break;
            case 1: {
                const auto reg = m_reg_set.top(IntervalHint::NOTHING);
                m_reg_set.push(reg);
                inst->init_temporal_regs(TemporalRegs(reg));
                break;
            }
            case 2: {
                const auto reg1 = m_reg_set.top(IntervalHint::NOTHING);
                const auto reg2 = m_reg_set.top(IntervalHint::NOTHING);
                m_reg_set.push(reg2);
                m_reg_set.push(reg1);
                inst->init_temporal_regs(TemporalRegs(reg1, reg2));
                break;
            }
            default: die("Unexpected number of temporal registers allocated: {}", temp_num);
        }
    }

    void do_stack_alloc(const LIRVal& lir_val) {
        if (const auto vreg = lir_val.assigned_reg().to_gp_op(); vreg.has_value()) {
            return;
        }

        lir_val.assign_reg(m_reg_set.stack_alloc(lir_val.size()));
    }

    void instruction_ordering() {
        const auto fn = [](const std::size_t acc, const LIRBlock* bb) { return acc + bb->size(); };
        const auto size = std::ranges::fold_left(m_preorder, 0UL, fn);

        m_instruction_ordering.reserve(size);
        for (const auto bb: m_preorder) {
            for (auto& inst: bb->instructions()) {
                m_instruction_ordering.push_back(&inst);
            }
        }
    }

    template<std::ranges::range Range>
    static aasm::GPRegSet collect_used_callee_saved_regs(Range&& args) {
        aasm::GPRegSet used_callee_saved_regs;
        for (const auto& reg: args) {
            const auto gp_vreg = reg.assigned_reg().to_gp_op();
            if (!gp_vreg.has_value()) {
                continue;
            }
            const auto gp_reg = gp_vreg->as_gp_reg();
            if (!gp_reg.has_value()) {
                continue;
            }

            used_callee_saved_regs.emplace(gp_reg.value());
        }

        return used_callee_saved_regs;
    }

    const LIRFuncData& m_obj_func_data;
    const LiveIntervals& m_intervals;
    const LiveIntervalsGroups& m_groups;
    const Ordering<LIRBlock>& m_preorder;

    details::VRegSelection m_reg_set;
    const call_conv::CallConvProvider* m_call_conv;

    aasm::GPRegSet m_used_callee_saved_regs{};

    std::vector<IntervalEntry> m_unhandled_intervals{};
    std::vector<IntervalEntry> m_inactive_intervals{};
    std::vector<IntervalEntry> m_active_intervals{};

    std::vector<LIRInstructionBase*> m_instruction_ordering{};
};