#pragma once

#include "LiveIntervals.h"
#include "base/analysis/AnalysisPass.h"
#include "lir/x64/analysis/liveness/LiveInfo.h"
#include "lir/x64/analysis/liveness/LivenessAnalysis.h"
#include "lir/x64/module/LIRBlock.h"
#include "lir/x64/module/LIRFuncData.h"


class LiveIntervalsEval final {
public:
    using result_type = LiveIntervals;
    using basic_block = LIRBlock;
    static constexpr auto analysis_kind = AnalysisType::LiveIntervalsEval;

private:
    explicit LiveIntervalsEval(const LIRFuncData &obj_func_data,
                               const LivenessAnalysisInfo &liveness,
                               const Ordering<LIRBlock> &ordering) noexcept: m_obj_func_data(obj_func_data),
                                                                              m_liveness(liveness),
                                                                              m_ordering(ordering) {}

public:
    void run() {
        setup_arguments();
        setup_locals();
        eval_usages();
    }

    std::unique_ptr<LiveIntervals> result() {
        LIRValMap<LiveInterval> all_intervals;

        const auto ordering = [](const LiveRange& a, const LiveRange& b) -> bool {
            return a.start() < b.start();
        };

        for (auto& [vreg, intervals]: m_intervals) {
            std::vector<LiveRange> intervals_for_vreg;
            intervals_for_vreg.reserve(intervals.size());
            for (const auto &interval: intervals | std::views::values) {
                intervals_for_vreg.emplace_back(interval);
            }

            std::ranges::sort(intervals_for_vreg, ordering);
            all_intervals.emplace(vreg, LiveInterval::create(std::move(intervals_for_vreg)));
        }

        return std::make_unique<LiveIntervals>(std::move(all_intervals));
    }

    static LiveIntervalsEval create(AnalysisPassManagerBase<LIRFuncData> *cache, const LIRFuncData *data) {
        const auto liveness = cache->analyze<LivenessAnalysis>(data);
        const auto preorder = cache->analyze<PreorderTraverseBase<LIRFuncData>>(data);

        return LiveIntervalsEval(*data, *liveness, *preorder);
    }

private:
    void setup_arguments() {
        const auto begin = m_obj_func_data.first();
        const auto& live_out = m_liveness.live_out(begin);
        const auto size = begin->size();
        for (const auto arg: m_obj_func_data.args()) {
            std::unordered_map<const LIRBlock*, LiveRange> intervals;
            if (live_out.contains(arg)) {
                intervals.emplace(begin, LiveRange(0, size));
            } else {
                intervals.emplace(begin, LiveRange(0, 0));
            }
            m_intervals.emplace(arg, std::move(intervals));
        }
    }

    void setup_locals() {
        std::uint32_t inst_number = -1;
        for (const auto bb: m_ordering) {
            const auto start = inst_number+1;
            const auto& live_out = m_liveness.live_out(bb);
            for (const auto& inst: bb->instructions()) {
                inst_number += 1;

                const auto defs_opt = LIRVal::try_from(&inst);
                if (!defs_opt.has_value()) {
                    continue; // Not a producer instruction.
                }

                for (const auto& def: defs_opt.value()) {
                    std::unordered_map<const LIRBlock*, LiveRange> intervals;
                    if (live_out.contains(def)) {
                        intervals.emplace(bb, LiveRange(inst_number, start + bb->size()));
                    } else {
                        intervals.emplace(bb, LiveRange(inst_number, inst_number));
                    }

                    m_intervals.emplace(def, std::move(intervals));
                }
            }
        }
    }

    void eval_usages() {
        std::uint32_t inst_number = -1;
        for (const auto bb: m_ordering) {
            const auto start = inst_number+1;
            const auto& live_out = m_liveness.live_out(bb);

            for (const auto& vreg: live_out) {
                auto& live_range = m_intervals.at(vreg);
                auto interval = live_range.find(bb);
                if (interval == live_range.end()) {
                    live_range.emplace(bb, LiveRange(start, start + bb->size()));
                } else {
                    interval->second.propagate(start + bb->size());
                }
            }

            for (const auto& inst: bb->instructions()) {
                inst_number += 1;
                for (const auto& in: inst.inputs()) {
                    const auto vreg_opt = LIRVal::try_from(in);
                    if (!vreg_opt.has_value()) {
                        continue;
                    }
                    const auto& vreg = vreg_opt.value();

                    auto& live_range = m_intervals.at(vreg);
                    auto interval = live_range.find(bb);
                    if (interval == live_range.end()) {
                        live_range.emplace(bb, LiveRange(start, inst_number));
                    } else {
                        interval->second.propagate(inst_number);
                    }
                }
            }
        }
    }

    const LIRFuncData& m_obj_func_data;
    const LivenessAnalysisInfo& m_liveness;
    const Ordering<LIRBlock>& m_ordering;
    LIRValMap<std::unordered_map<const LIRBlock*, LiveRange>> m_intervals{};
};
