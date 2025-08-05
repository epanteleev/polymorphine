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
        for (const auto& [lir, rax_reg]: m_fixed_registers) {
            m_reg_allocation.emplace(lir, rax_reg);

            // Activate group
            const auto group_opt = m_groups.try_get_group(lir);
            if (group_opt.has_value()) {
                m_group_reg_mapping.emplace(group_opt.value(), rax_reg);
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
                if (real_interval->start() < unhandled_interval->finish()) {
                    break;
                }

                const auto fixed_reg_groups = m_groups.try_get_fixed_register(unhandled.m_vreg);
                if (!fixed_reg_groups.has_value()) {
                    continue;
                }

                if (!real_interval->intersects(*unhandled_interval)) {
                    continue;
                }

                m_active_intervals.emplace_back(unhandled);
                if (const auto reg_opt = fixed_reg_groups.value().as_gp_reg(); reg_opt.has_value()) {
                    m_reg_set.remove(reg_opt.value());
                }
            }

            allocate_vreg(m_reg_set, vreg);
            m_active_intervals.emplace_back(unhandled_interval, vreg);
        }
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

    void allocate_vreg(details::RegSet<CC>& reg_set, const LIRVal& vreg) {
        const auto group_opt = m_groups.try_get_group(vreg);
        if (!group_opt.has_value()) {
            if (const auto pair = m_reg_allocation.try_emplace(vreg, reg_set.top()); pair.second) {
                reg_set.pop();
            }

            return;
        }

        const auto group = group_opt.value();
        if (const auto it = m_group_reg_mapping.find(group); it != m_group_reg_mapping.end()) {
            // This group already has a register allocated.
            m_reg_allocation.try_emplace(vreg, it->second);
            return;
        }

        // Allocate a register for the group.
        const auto reg = reg_set.top();
        const auto pair = m_reg_allocation.try_emplace(vreg, reg_set.top());
        if (pair.second) reg_set.pop();

        m_group_reg_mapping.emplace(group, reg);
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

    std::unordered_map<const Group*, GPVReg> m_group_reg_mapping{};

    std::vector<IntervalEntry> m_unhandled_intervals{};
    std::vector<IntervalEntry> m_inactive_intervals{};
    std::vector<IntervalEntry> m_active_intervals{};
};
