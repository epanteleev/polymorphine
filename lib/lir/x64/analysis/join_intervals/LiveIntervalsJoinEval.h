#pragma once

#include "LiveIntervalsGroups.h"
#include "base/analysis/AnalysisPass.h"
#include "lir/x64/analysis/intervals/IntervalHint.h"
#include "lir/x64/asm/GPVRegMap.h"
#include "lir/x64/instruction/Matcher.h"
#include "lir/x64/module/LIRBlock.h"
#include "lir/x64/operand/OperandMatcher.h"

template<call_conv::CallConv CC>
class LiveIntervalsJoinEval final {
public:
    using result_type = LiveIntervalsGroups;
    using basic_block = LIRBlock;
    static constexpr auto analysis_kind = AnalysisType::LiveIntervalsGroups;

private:
    LiveIntervalsJoinEval(const LiveIntervals& intervals, const FixedRegisters& fixed_regs, const LIRFuncData& data) noexcept:
        m_intervals(intervals),
        m_fixed_regs(fixed_regs), m_data(data) {}

public:
    void run() {
        setup_fixed_reg_groups();
        setup_parallel_copy_groups();
        do_joining();
    }

    std::unique_ptr<LiveIntervalsGroups> result() {
        return std::make_unique<LiveIntervalsGroups>(std::move(m_groups), std::move(m_group_mapping));
    }

    static LiveIntervalsJoinEval create(AnalysisPassManagerBase<LIRFuncData> *cache, const LIRFuncData *data) {
        const auto intervals = cache->analyze<LiveIntervalsEval>(data);
        const auto fixed_regs = cache->analyze<FixedRegistersEvalBase<CC>>(data);
        return {*intervals, *fixed_regs, *data};
    }

private:
    void setup_fixed_reg_groups() {
        GPVRegMap<std::vector<LIRVal>> m_reg_to_lir_val;
        // Collect all LIR values that are mapped to fixed registers.
        for (auto& [lir_val, fixed_reg]: m_fixed_regs) {
            auto [vec, _] = m_reg_to_lir_val.try_emplace(fixed_reg, std::vector<LIRVal>{});
            vec->second.push_back(lir_val);
        }

        for (auto& [fixed_reg, lir_values]: m_reg_to_lir_val) {
            add_group(create_live_intervals(lir_values), std::move(lir_values), fixed_reg);
        }
    }

    void setup_parallel_copy_groups() {
        for (const auto& bb: m_data.basic_blocks()) {
            for (const auto& inst: bb.instructions()) {
                if (!inst.isa(parallel_copy())) {
                    break;
                }

                const auto& parallel_copy = dynamic_cast<const ParallelCopy&>(inst);
                auto inputs = parallel_copy_inputs(parallel_copy);
                auto new_intervals = create_live_intervals(inputs);
                add_group(std::move(new_intervals), std::move(inputs), std::nullopt);
            }
        }
    }

    void do_joining() {
        for (const auto& [lir_val, interval]: m_intervals.intervals()) {
            if (lir_val.isa(gen())) {
                // Skip stack alloc values
                continue;
            }

            if (const auto group = m_group_mapping.find(lir_val); group != m_group_mapping.end()) {
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

                group->add_member(lir_val, interval);
                m_group_mapping.emplace(lir_val, group);
                break;
            }
        }
    }

    void add_group(LiveInterval&& live_interval, std::vector<LIRVal>&& members, const std::optional<GPVReg> &fixed_register) {
        m_groups.emplace_back(std::move(live_interval), std::move(members), fixed_register);
        for (const auto lir_val: m_groups.back().members()) {
            m_group_mapping.emplace(lir_val, std::prev(m_groups.end()));
        }
    }

    LiveInterval create_live_intervals(const std::span<LIRVal>& lir_values) const noexcept {
        std::vector<LiveRange> new_intervals;
        new_intervals.reserve(lir_values.size());

        auto acc = IntervalHint::CALL_LIVE_OUT;
        for (const auto& lir_val: lir_values) {
            const auto& intervals = m_intervals.intervals(lir_val);
            acc = join_hints(acc, intervals.hint());

            for (auto& range: intervals) {
                new_intervals.emplace_back(range);
            }
        }

        return LiveInterval::create(std::move(new_intervals), acc);
    }

    static std::vector<LIRVal> parallel_copy_inputs(const ParallelCopy& parallel_copy) {
        std::vector<LIRVal> new_lir_vals;
        new_lir_vals.reserve(parallel_copy.inputs().size()+1);
        for (const auto& lir_op: parallel_copy.inputs()) {
            const auto lir_v = LIRVal::try_from(lir_op);
            if (!lir_v.has_value()) {
                continue;
            }
            if (!lir_v->isa(copy_v())) {
                continue;
            }

            new_lir_vals.push_back(lir_v.value());
        }
        new_lir_vals.push_back(parallel_copy.def(0));
        return new_lir_vals;
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
    const LIRFuncData& m_data;

    std::deque<Group> m_groups;
    LIRValMap<LiveIntervalsGroups::group_iterator> m_group_mapping{};
};