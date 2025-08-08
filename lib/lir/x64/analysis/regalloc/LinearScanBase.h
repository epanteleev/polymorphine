#pragma once

#include "RegisterAllocation.h"
#include "VRegSelection.h"

#include "lir/x64/analysis/fixedregs/FixedRegistersEvalBase.h"
#include "lir/x64/analysis/intervals/LiveIntervals.h"
#include "lir/x64/analysis/intervals/LiveIntervalsEval.h"
#include "lir/x64/analysis/join_intervals/LiveIntervalsJoinEval.h"
#include "lir/x64/operand/OperandMatcher.h"
#include "lir/x64/asm/cc/CallConv.h"


template<call_conv::CallConv CC>
class LinearScanBase final {
public:
    using result_type = RegisterAllocation;
    using basic_block = LIRBlock;
    static constexpr auto analysis_kind = AnalysisType::LinearScan;

private:
    LinearScanBase(const LIRFuncData &obj_func_data, details::VRegSelection<CC>&& reg_set, const LiveIntervals& intervals, const LiveIntervalsGroups& groups) noexcept:
        m_obj_func_data(obj_func_data),
        m_intervals(intervals),
        m_groups(groups),
        m_reg_set(std::move(reg_set)) {}

    struct IntervalEntry final {
        IntervalEntry(const LiveInterval* interval, const LIRVal vreg) noexcept:
            m_interval(interval),
            m_vreg(vreg) {}

        const LiveInterval* m_interval;
        LIRVal m_vreg;
    };

public:
    void run() {
        m_used_callee_saved_regs.reserve(CC::GP_CALLEE_SAVE_REGISTERS.size());

        allocate_fixed_registers();
        setup_unhandled_intervals();
        do_register_allocation();
    }

    std::unique_ptr<result_type> result() noexcept {
        return std::make_unique<RegisterAllocation>(std::move(m_reg_allocation), std::move(m_used_callee_saved_regs), m_reg_set.local_area_size());
    }

    static LinearScanBase create(AnalysisPassManagerBase<LIRFuncData> *cache, const LIRFuncData *data) {
        const auto fixed_registers = cache->analyze<FixedRegistersEvalBase<CC>>(data);
        const auto intervals = cache->analyze<LiveIntervalsEval>(data);
        const auto joins = cache->analyze<LiveIntervalsJoinEval<CC>>(data);
        return {*data, details::VRegSelection<CC>::create(fixed_registers->used_argument_registers()), *intervals, *joins};
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
            return lhs.m_interval->begin()->start() > rhs.m_interval->begin()->start();
        };

        std::ranges::sort(m_unhandled_intervals, pred);
    }

    void do_register_allocation() {
        while (!m_unhandled_intervals.empty()) {
            auto [unhandled_interval, vreg] = m_unhandled_intervals.back();
            m_unhandled_intervals.pop_back();

            if (vreg.isa(gen())) {
                do_stack_alloc(vreg);
                continue;
            }

            const auto active_eraser = [&](const IntervalEntry& entry) {
                const auto real_interval = get_real_interval(entry);
                const auto reg = m_reg_allocation.at(entry.m_vreg);
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

            remove_interval_if(m_active_intervals, active_eraser);

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
                const auto reg = m_reg_allocation.at(entry.m_vreg);
                if (const auto reg_opt = reg.as_gp_reg(); reg_opt.has_value()) {
                    m_reg_set.remove(reg_opt.value());
                }

                return true;
            };

            remove_interval_if(m_inactive_intervals, unactive_eraser);

            for (const auto& unhandled: std::ranges::reverse_view(m_unhandled_intervals)) {
                const auto group = m_groups.try_get_group(unhandled.m_vreg);
                if (!group.has_value()) {
                    // No group for this vreg, we can skip it.
                    continue;
                }

                const LiveInterval* real_interval = &group.value()->m_interval;
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

            select_vreg(vreg);
            m_active_intervals.emplace_back(unhandled_interval, vreg);
        }
    }

    const LiveInterval* get_real_interval(const IntervalEntry& entry) const {
        if (const auto it = m_groups.try_get_group(entry.m_vreg); it.has_value()) {
            return &it.value()->m_interval;
        }

        return entry.m_interval;
    }

    void select_vreg(const LIRVal& vreg) {
        if (m_reg_allocation.contains(vreg)) {
            return;
        }
        const auto group = m_groups.try_get_group(vreg);
        if (group.has_value()) {
            assertion(!group.value()->fixed_register().has_value(), "Group with fixed register should not be allocated here");
            const auto reg = m_reg_set.top();
            for (const auto& group_vreg: group.value()->m_values) {
                allocate_register(group_vreg, reg);
            }
            return;
        }

        const auto reg = m_reg_set.top();
        allocate_register(vreg, reg);
    }

    void allocate_register(const LIRVal& vreg, const GPVReg& reg) {
        if (const auto gp_reg= reg.as_gp_reg(); gp_reg.has_value()) {
            allocate_register(vreg, gp_reg.value());
            return;
        }

        auto [_, has] = m_reg_allocation.try_emplace(vreg, reg);
        assertion(has, "Register already allocated for LIRVal");
    }

    void allocate_register(const LIRVal& vreg, const aasm::GPReg reg) {
        auto [_, has] = m_reg_allocation.try_emplace(vreg, reg);
        assertion(has, "Register already allocated for LIRVal");
        if (!std::ranges::contains(CC::GP_CALLEE_SAVE_REGISTERS, reg)) {
            // This is a caller-save register, we can skip it.
            return;
        }

        if (std::ranges::contains(m_used_callee_saved_regs, reg)) {
            return; // Already added
        }

        m_used_callee_saved_regs.push_back(reg);
    }

    void do_stack_alloc(const LIRVal& vreg) {
        auto [_, has] = m_reg_allocation.try_emplace(vreg, m_reg_set.stack_alloc(vreg.size()));
        assertion(has, "Register already allocated for LIRVal");
    }

    template<typename Fn>
    static void remove_interval_if(std::vector<IntervalEntry>& intervals, Fn&& fn) {
        for (auto it = intervals.begin(); it != intervals.end();) {
            if (fn(*it)) {
                std::swap(*it, intervals.back());
                intervals.pop_back();
            } else {
                ++it;
            }
        }
    }

    const LIRFuncData& m_obj_func_data;
    const LiveIntervals& m_intervals;
    const LiveIntervalsGroups& m_groups;

    details::VRegSelection<CC> m_reg_set;
    std::vector<aasm::GPReg> m_used_callee_saved_regs{};
    LIRValMap<GPVReg> m_reg_allocation{};

    std::vector<IntervalEntry> m_unhandled_intervals{};
    std::vector<IntervalEntry> m_inactive_intervals{};
    std::vector<IntervalEntry> m_active_intervals{};
};
