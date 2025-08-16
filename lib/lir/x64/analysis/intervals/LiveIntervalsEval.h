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
        for (auto& [lir_val, intervals]: m_intervals) {
            std::vector<LiveRange> intervals_for_lir_val;
            intervals_for_lir_val.reserve(intervals.size());
            for (const auto &interval: intervals | std::views::values) {
                intervals_for_lir_val.emplace_back(interval);
            }

            all_intervals.emplace(lir_val, LiveInterval::create(std::move(intervals_for_lir_val), get_hint(lir_val)));
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
        for (const auto arg: m_obj_func_data.args()) {
            std::unordered_map<const LIRBlock*, LiveRange> intervals;
            intervals.emplace(begin, LiveRange(0, 0));
            m_intervals.emplace(arg, std::move(intervals));
        }
    }

    void setup_locals() {
        std::uint32_t inst_number{};
        for (const auto bb: m_ordering) {
            for (const auto& inst: bb->instructions()) {
                inst_number += 1;
                for (const auto& def: LIRVal::defs(&inst)) {
                    std::unordered_map<const LIRBlock*, LiveRange> intervals;
                    intervals.emplace(bb, LiveRange(inst_number, inst_number));
                    m_intervals.emplace(def, std::move(intervals));
                }
            }
        }
    }

    void eval_usages() {
        std::uint32_t inst_number{};
        for (const auto bb: m_ordering) {
            const auto start = inst_number+1;
            const auto call_terminator = bb->last()->isa(call());

            for (const auto& lir_val: m_liveness.live_out(bb)) {
                auto& live_range = m_intervals.at(lir_val);
                const auto interval = live_range.find(bb);
                if (interval == live_range.end()) {
                    live_range.emplace(bb, LiveRange(start, start + bb->size()-1));
                    continue;
                }

                if (call_terminator) {
                    m_call_live_out.emplace(lir_val);
                }

                interval->second.propagate(start + bb->size());
            }

            for (const auto& inst: bb->instructions()) {
                inst_number += 1;
                for (const auto& in: inst.inputs()) {
                    const auto lir_val_opt = LIRVal::try_from(in);
                    if (!lir_val_opt.has_value()) {
                        continue;
                    }

                    const auto& lir_val = lir_val_opt.value();
                    auto& live_range = m_intervals.at(lir_val);
                    if (auto interval = live_range.find(bb); interval == live_range.end()) {
                        live_range.emplace(bb, LiveRange(start, inst_number));

                    } else {
                        interval->second.propagate(inst_number);
                    }
                }
            }
        }
    }

    IntervalHint get_hint(const LIRVal& lir_val) const noexcept {
        if (m_call_live_out.contains(lir_val)) {
            return IntervalHint::CALL_LIVE_OUT;
        }

        return IntervalHint::NOTHING;
    }

    const LIRFuncData& m_obj_func_data;
    const LivenessAnalysisInfo& m_liveness;
    const Ordering<LIRBlock>& m_ordering;
    LIRValMap<std::unordered_map<const LIRBlock*, LiveRange>> m_intervals{};
    LIRValSet m_call_live_out{};
};
