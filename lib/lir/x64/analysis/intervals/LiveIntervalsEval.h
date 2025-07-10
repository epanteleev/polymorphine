#pragma once

#include "LiveIntervals.h"
#include "base/analysis/AnalysisPass.h"
#include "lir/x64/analysis/liveness/LiveInfo.h"
#include "lir/x64/analysis/liveness/LivenessAnalysis.h"
#include "lir/x64/module/MachBlock.h"
#include "lir/x64/module/ObjFuncData.h"


class LiveIntervalsEval final {
public:
    using result_type = LiveIntervals;
    using basic_block = MachBlock;
    static constexpr auto analysis_kind = AnalysisType::LiveIntervalsEval;

private:
    explicit LiveIntervalsEval(const ObjFuncData &obj_func_data,
                               const LivenessAnalysisInfo &liveness,
                               const Ordering<MachBlock> &ordering) noexcept: m_obj_func_data(obj_func_data),
                                                                              m_liveness(liveness),
                                                                              m_ordering(ordering) {}

public:
    void run() {
        setup_arguments();
        setup_locals();
        eval_usages();
    }

    std::unique_ptr<LiveIntervals> result() {
        return std::make_unique<LiveIntervals>(std::move(m_intervals));
    }

    static LiveIntervalsEval create(AnalysisPassCacheBase<ObjFuncData> *cache, const ObjFuncData *data) {
        const auto liveness = cache->analyze<LivenessAnalysis>(data);
        const auto preoder = cache->analyze<PreorderTraverseBase<ObjFuncData>>(data);

        return LiveIntervalsEval(*data, *liveness, *preoder);
    }

private:
    void setup_arguments() {
        const auto begin = m_obj_func_data.first();
        const auto& live_out = m_liveness.live_out(begin);
        const auto size = begin->size();
        for (const auto& arg: m_obj_func_data.args()) {
            const auto vreg = VReg::from(&arg);
            std::unordered_map<const MachBlock*, Interval> intervals;
            if (live_out.contains(vreg)) {
                intervals.emplace(begin, Interval(0, size));
            } else {
                intervals.emplace(begin, Interval(0, 0));
            }
            m_intervals.emplace(vreg, std::move(intervals));
        }
    }

    void setup_locals() {
        std::uint32_t inst_number = -1;
        for (const auto bb: m_ordering) {
            const auto start = inst_number+1;
            const auto& live_out = m_liveness.live_out(bb);
            for (const auto& inst: bb->instructions()) {
                inst_number += 1;

                for (const auto& def: inst.defs()) {
                    std::unordered_map<const MachBlock*, Interval> intervals;
                    if (live_out.contains(def)) {
                        intervals.emplace(bb, Interval(inst_number, start + bb->size()));
                    } else {
                        intervals.emplace(bb, Interval(inst_number, inst_number));
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
                    live_range.emplace(bb, Interval(start, start + bb->size()));
                } else {
                    interval->second.propagate(start + bb->size());
                }
            }

            for (const auto& inst: bb->instructions()) {
                inst_number += 1;
                for (const auto& in: inst.inputs()) {
                    const auto vreg_opt = VReg::try_from(in);
                    if (!vreg_opt.has_value()) {
                        continue;
                    }
                    const auto& vreg = vreg_opt.value();

                    auto& live_range = m_intervals.at(vreg);
                    auto interval = live_range.find(bb);
                    if (interval == live_range.end()) {
                        live_range.emplace(bb, Interval(start, start + bb->size()));
                    } else {
                        interval->second.propagate(start + bb->size());
                    }
                }
            }
        }
    }

    const ObjFuncData& m_obj_func_data;
    const LivenessAnalysisInfo& m_liveness;
    const Ordering<MachBlock>& m_ordering;
    VRegMap<std::unordered_map<const MachBlock*, Interval>> m_intervals{};
};
