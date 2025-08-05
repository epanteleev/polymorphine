#pragma once

#include "LiveIntervalsGroups.h"
#include "base/analysis/AnalysisPass.h"
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

    struct IntervalEntry final {
        IntervalEntry(const LiveInterval* interval, const LIRVal vreg) noexcept:
            m_interval(interval),
            m_vreg(vreg) {}

        const LiveInterval* m_interval;
        LIRVal m_vreg;
    };

public:
    void run() {
        setup_intervals();
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
    void setup_intervals() {
        m_unhandled_intervals.reserve(m_intervals.intervals().size());
        for (auto& [lir_val, interval]: m_intervals.intervals()) {
            if (lir_val.arg().has_value()) {
                continue; // Do not handle arguments.
            }

            m_unhandled_intervals.emplace_back(&interval, lir_val);
        }

        const auto pred = [](const IntervalEntry& lhs, const IntervalEntry& rhs) {
            return lhs.m_interval->begin()->start() > rhs.m_interval->begin()->start();
        };

        std::ranges::sort(m_unhandled_intervals, pred);
    }

    void do_joining() {
        while (!m_unhandled_intervals.empty()) {
            const auto current = m_unhandled_intervals.back();
            m_unhandled_intervals.pop_back();

            if (current.m_vreg.isa(gen())) {
                continue;
            }

            if (has_intersected_fixed_register(current)) {
                continue;
            }

            for (const auto& unhandled: std::ranges::reverse_view(m_unhandled_intervals)) {
                if (unhandled.m_vreg.isa(gen())) {
                    continue;
                }

                const auto actual_interval = try_get_group_interval(current);
                const auto entry_actual_interval = try_get_group_interval(unhandled);
                if (entry_actual_interval->start() > actual_interval->finish()) {
                    // No need to check further, the intervals are sorted.
                    break;
                }

                if (entry_actual_interval->intersects(*actual_interval)) {
                    continue;
                }

                if (join_and_erase_active_intervals(unhandled, current)) {
                    break;
                }
            }
        }
    }

    bool has_intersected_fixed_register(const IntervalEntry& activated) const {
        const auto actual_interval = try_get_group_interval(activated);
        const auto fixed_reg2 = try_get_fixed_register(activated.m_vreg);

        for (const auto& unhandled: std::ranges::reverse_view(m_unhandled_intervals)) {
            if (unhandled.m_vreg.isa(gen())) {
                continue;
            }

            const auto entry_actual_interval = try_get_group_interval(unhandled);
            if (entry_actual_interval->start() > actual_interval->finish()) {
                // No need to check further, the intervals are sorted.
                break;
            }

            if (!entry_actual_interval->intersects(*actual_interval)) {
                continue;
            }

            const auto fixed_reg1 = try_get_fixed_register(unhandled.m_vreg);
            if (fixed_reg1 == fixed_reg2) {
                return true;
            }
        }

        return false;
    }

    /**
     * Joins the given entry with the unhandled interval if they do not intersect.
     * @return false if the entries were joined, true if they were not.
     */
    bool join_and_erase_active_intervals(const IntervalEntry& first, const IntervalEntry& second) {
        const auto fixed_reg1 = try_get_fixed_register(first.m_vreg);
        const auto fixed_reg2 = try_get_fixed_register(second.m_vreg);

        if (fixed_reg1 == fixed_reg2) {
            // Both intervals do not have a fixed register, we can join them.
            register_group(first, second, fixed_reg1);
            return false;
        }
        if (!fixed_reg1.has_value() && fixed_reg2.has_value()) {
            register_group(first, second, fixed_reg2);
            return false;
        }
        if (fixed_reg1.has_value() && !fixed_reg2.has_value()) {
            register_group(first, second, fixed_reg1);
            return false;
        }

        return true;
    }

    const LiveInterval * try_get_group_interval(const IntervalEntry& entry) const {
        if (const auto it = m_group_mapping.find(entry.m_vreg); it != m_group_mapping.end()) {
            // This interval is part of a group, we need to merge it with the group.
            return &it->second->m_interval;
        }

        return entry.m_interval;
    }

    std::optional<GPVReg> try_get_fixed_register(const LIRVal& vreg) const {
        if (const auto it = m_group_mapping.find(vreg); it != m_group_mapping.end()) {
            return it->second->m_fixed_register;
        }

        return m_fixed_regs.get(vreg);
    }

    void register_group(const IntervalEntry& first, const IntervalEntry& second, const std::optional<GPVReg>& fixed_reg) {
        if (const auto it = m_group_mapping.find(first.m_vreg); it != m_group_mapping.end()) {
            auto& group = *it->second;
            group.m_values.push_back(second.m_vreg);
            group.merge_interval(*second.m_interval);
            m_group_mapping.emplace(second.m_vreg, it->second);

        } else if (const auto it2 = m_group_mapping.find(second.m_vreg); it2 != m_group_mapping.end()) {
            auto& group = *it2->second;
            group.m_values.push_back(first.m_vreg);
            group.merge_interval(*first.m_interval);
            m_group_mapping.emplace(first.m_vreg, it2->second);

        } else {
            std::vector<LiveRange> intervals;
            intervals.reserve(first.m_interval->size() + second.m_interval->size());
            for (const auto& interval : *first.m_interval) {
                intervals.emplace_back(interval);
            }
            for (const auto& interval : *second.m_interval) {
                intervals.emplace_back(interval);
            }

            // Neither vreg1 nor vreg2 are in a group, we need to create a new group.
            m_groups.emplace_back(LiveInterval::create(std::move(intervals)), std::vector{first.m_vreg, second.m_vreg}, fixed_reg);
            m_group_mapping.emplace(first.m_vreg, std::prev(m_groups.end()));
            m_group_mapping.emplace(second.m_vreg, std::prev(m_groups.end()));
        }
    }

    const LiveIntervals& m_intervals;
    const FixedRegisters& m_fixed_regs;

    std::vector<IntervalEntry> m_unhandled_intervals;

    std::deque<Group> m_groups;
    LIRValMap<LiveIntervalsGroups::group_iterator> m_group_mapping{};
};
