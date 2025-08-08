#pragma once

#include "LiveIntervalsGroups.h"
#include "asm/reg/RegMap.h"
#include "base/analysis/AnalysisPass.h"
#include "lir/x64/asm/GPVRegMap.h"
#include "lir/x64/module/LIRBlock.h"
#include "lir/x64/operand/OperandMatcher.h"

template<call_conv::CallConv CC>
class LiveIntervalsJoinEval final {
public:
    using result_type = LiveIntervalsGroups;
    using basic_block = LIRBlock;
    static constexpr auto analysis_kind = AnalysisType::LiveIntervalsGroups;

private:
    LiveIntervalsJoinEval(const LiveIntervals& intervals, const FixedRegisters& fixed_regs) noexcept:
        m_intervals(intervals),
        m_fixed_regs(fixed_regs) {}

public:
    void run() {
        setup_groups();
        do_joining();
    }

    std::unique_ptr<LiveIntervalsGroups> result() {
        return std::make_unique<LiveIntervalsGroups>(std::move(m_groups), std::move(m_group_mapping));
    }

    static LiveIntervalsJoinEval create(AnalysisPassManagerBase<LIRFuncData> *cache, const LIRFuncData *data) {
        const auto intervals = cache->analyze<LiveIntervalsEval>(data);
        const auto fixed_regs = cache->analyze<FixedRegistersEvalBase<CC>>(data);
        return {*intervals, *fixed_regs};
    }

private:
    void setup_groups() {
        GPVRegMap<std::vector<LIRVal>> m_reg_to_lir_val;
        // Collect all LIR values that are mapped to fixed registers.
        for (auto& [lir_val, fixed_reg]: m_fixed_regs) {
            auto [vec, _] = m_reg_to_lir_val.try_emplace(fixed_reg, std::vector<LIRVal>{});
            vec->second.push_back(lir_val);
        }

        for (auto& [fixed_reg, lir_values]: m_reg_to_lir_val) {
            std::vector<LiveRange> new_intervals;
            new_intervals.reserve(lir_values.size());

            for (const auto& lir_val: lir_values) {
                for (auto& interval: m_intervals.intervals(lir_val)) {
                    new_intervals.emplace_back(interval);
                }
            }

            m_groups.emplace_back(LiveInterval::create(std::move(new_intervals)), std::move(lir_values), fixed_reg);
            for (const auto lir_val: m_groups.back().members()) {
                m_group_mapping.emplace(lir_val, std::prev(m_groups.end()));
            }
        }
    }

    void do_joining() {
        for (const auto& [vreg, interval]: m_intervals.intervals()) {
            if (vreg.isa(gen())) {
                // Skip stack alloc values
                continue;
            }

            const auto group_opt = m_group_mapping.find(vreg);
            if (group_opt != m_group_mapping.end()) {
                // This interval is already part of a group, we can skip it.
                continue;
            }

            for (auto group = m_groups.begin(); group != m_groups.end(); ++group) {
                if (contains_fixed_mem_slot(*group)) {
                    // The group has a fixed vreg on stack slot. Do not join it.
                    continue;
                }

                if (!interval.follows(group->m_interval)) {
                    // No way to join intervals. Skip it
                    continue;
                }

                group->add_member(vreg, interval);
                m_group_mapping.emplace(vreg, group);
                break;
            }
        }
    }

    static bool contains_fixed_mem_slot(const Group& group) noexcept {
        const auto& reg = group.fixed_register();
        if (!reg.has_value()) {
            return false;
        }

        const auto& mem = reg.value().as_address();
        return mem.has_value();
    }

    const LiveIntervals& m_intervals;
    const FixedRegisters& m_fixed_regs;

    std::deque<Group> m_groups;
    LIRValMap<LiveIntervalsGroups::group_iterator> m_group_mapping{};
};