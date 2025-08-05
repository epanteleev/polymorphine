#pragma once

#include "RegisterAllocation.h"
#include "RegSet.h"

#include "lir/x64/analysis/fixedregs/FixedRegistersEvalBase.h"
#include "lir/x64/analysis/intervals/LiveIntervals.h"
#include "lir/x64/analysis/intervals/LiveIntervalsEval.h"
#include "lir/x64/analysis/join_intervals/LiveIntervalsJoinEval.h"
#include "lir/x64/operand/OperandMatcher.h"
#include "lir/x64/asm/cc/CallConv.h"

#include "utility/Align.h"

template<call_conv::CallConv CC>
class LinearScanBase final {
public:
    using result_type = RegisterAllocation;
    using basic_block = LIRBlock;
    static constexpr auto analysis_kind = AnalysisType::LinearScan;

private:
    LinearScanBase(const LIRFuncData &obj_func_data, const FixedRegisters& fixed_registers, const LiveIntervals& intervals, const LiveIntervalsGroups& groups) noexcept:
        m_obj_func_data(obj_func_data),
        m_fixed_registers(fixed_registers),
        m_intervals(intervals),
        m_groups(groups),
        m_reg_set(details::RegSet<CC>::create(m_fixed_registers.arguments())) {}

    struct IntervalEntry final {
        IntervalEntry(const LiveInterval* interval, const LIRVal vreg) noexcept:
            m_interval(interval),
            m_vreg(vreg) {}

        const LiveInterval* m_interval;
        LIRVal m_vreg;
    };

public:
    void run() {
        allocate_fixed_registers();
        setup_unhandled_intervals();
        do_register_allocation();
    }

    std::unique_ptr<result_type> result() noexcept {
        return std::make_unique<RegisterAllocation>(std::move(m_reg_allocation), m_local_area_size);
    }

    static LinearScanBase create(AnalysisPassManagerBase<LIRFuncData> *cache, const LIRFuncData *data) {
        const auto fixed_registers = cache->analyze<FixedRegistersEvalBase<CC>>(data);
        const auto intervals = cache->analyze<LiveIntervalsEval>(data);
        const auto joins = cache->analyze<LiveIntervalsJoinEval<CC>>(data);
        return {*data, *fixed_registers, *intervals, *joins};
    }

private:
    void allocate_fixed_registers() {
        for (const auto& [lir, reg]: m_fixed_registers) {
            m_reg_allocation.emplace(lir, reg);
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
                if (real_interval->intersects(*unhandled_interval)) {
                    return false;
                }
                m_inactive_intervals.emplace_back(entry);
                const auto reg = m_reg_allocation.at(entry.m_vreg);
                if (const auto reg_opt = reg.as_gp_reg(); reg_opt.has_value()) {
                    m_reg_set.push(reg_opt.value());
                }

                return true;
            };

            remove_interval_if(m_active_intervals, active_eraser);

            const auto unactive_eraser = [&](const IntervalEntry& entry) {
                const auto real_interval = get_real_interval(entry);
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
                const auto real_interval = get_real_interval(unhandled);

                const auto fixed_reg_groups = try_get_fixed_register(unhandled.m_vreg);
                if (!fixed_reg_groups.has_value()) {
                    continue;
                }

                if (!real_interval->intersects(*unhandled_interval)) {
                    continue;
                }

                alloc_fixed_reg_for_group(unhandled.m_vreg);
                m_active_intervals.emplace_back(unhandled);
                if (const auto reg_opt = fixed_reg_groups.value().as_gp_reg(); reg_opt.has_value()) {
                    m_reg_set.remove(reg_opt.value());
                }
            }

            allocate_vreg(vreg);
            m_active_intervals.emplace_back(unhandled_interval, vreg);
        }
    }

    std::optional<GPVReg> try_get_fixed_register(const LIRVal& vreg) const {
        if (const auto group_opt = m_groups.try_get_fixed_register(vreg); group_opt.has_value()) {
            return group_opt;
        }

        return  m_fixed_registers.get(vreg);
    }

    const LiveInterval* get_real_interval(const IntervalEntry& entry) const {
        if (const auto it = m_groups.try_get_group(entry.m_vreg); it.has_value()) {
            return &it.value()->m_interval;
        }

        return entry.m_interval;
    }

    template<typename Fn>
    void remove_interval_if(std::vector<IntervalEntry>& intervals, Fn&& fn) {
        for (auto it = intervals.begin(); it != intervals.end();) {
            if (fn(*it)) {
                std::swap(*it, intervals.back());
                intervals.pop_back();
            } else {
                ++it;
            }
        }
    }

    void alloc_fixed_reg_for_group(const LIRVal& vreg) {
        const auto group = m_groups.try_get_group(vreg);
        if (!group.has_value()) {
            return;
        }

        const auto group_reg = m_groups.try_get_fixed_register(vreg);
        if (!group_reg.has_value()) {
            return;
        }

        for (const auto& group_vreg: group.value()->m_values) {
            m_reg_allocation.emplace(group_vreg, group_reg.value());
        }
    }

    void allocate_vreg(const LIRVal& vreg) {
        if (m_reg_allocation.contains(vreg)) {
            return;
        }
        const auto group = m_groups.try_get_group(vreg);
        if (group.has_value()) {
            const auto group_reg = m_groups.try_get_fixed_register(vreg);
            if (group_reg.has_value()) {
                for (const auto& group_vreg: group.value()->m_values) {
                    m_reg_allocation.emplace(group_vreg, group_reg.value());
                }

            } else {
                const auto reg = m_reg_set.top();
                for (const auto& group_vreg: group.value()->m_values) {
                    m_reg_allocation.emplace(group_vreg, reg);
                }
                m_reg_set.pop();
            }
            return;
        }

        const auto reg = m_reg_set.top();
        const auto pair = m_reg_allocation.try_emplace(vreg, reg);
        if (pair.second) m_reg_set.pop();
    }

    void do_stack_alloc(const LIRVal& vreg) {
        const auto offset = align_up(m_local_area_size, vreg.size()) + vreg.size();
        m_reg_allocation.try_emplace(vreg, aasm::Address(aasm::rbp, -offset));
        m_local_area_size = offset;
    }

    const LIRFuncData& m_obj_func_data;
    const FixedRegisters& m_fixed_registers;
    const LiveIntervals& m_intervals;
    const LiveIntervalsGroups& m_groups;

    details::RegSet<CC> m_reg_set;

    LIRValMap<GPVReg> m_reg_allocation{};
    // Size of the 'gen' values in the local area.
    std::int32_t m_local_area_size{0};

    std::vector<IntervalEntry> m_unhandled_intervals{};
    std::vector<IntervalEntry> m_inactive_intervals{};
    std::vector<IntervalEntry> m_active_intervals{};
};
