#pragma once

#include "LiveInfo.h"
#include "base/analysis/AnalysisPassManagerBase.h"
#include "base/analysis/traverse/PreorderTraverseBase.h"

#include "lir/x64/module/LIRBlock.h"
#include "lir/x64/instruction/Matcher.h"
#include "lir/x64/module/LIRFuncData.h"
#include "lir/x64/operand/LIRValMap.h"


class LivenessAnalysis final {
public:
    using result_type = LivenessAnalysisInfo;
    using basic_block = LIRBlock;
    static constexpr auto analysis_kind = AnalysisType::LivenessAnalysis;

private:
    explicit LivenessAnalysis(const Ordering<LIRBlock> &ordering):
        m_ordering(ordering) {}

public:
    void run() {
        compute_local_live_set();
        setup_liveness();

        auto changed = false;
        do {
            for (const auto& bb: m_ordering) {
                LIRValSet live_out;
                for (const auto succ: bb->successors()) {
                    // live_out = b.live_out ∪ succ.live_in
                    const auto live_in_succ = m_liveness.find(succ);
                    if (live_in_succ == m_liveness.end()) {
                        continue;
                    }

                    for (const auto& live_in_reg: live_in_succ->second.first) {
                        live_out.insert(live_in_reg);
                    }
                }

                changed = add_all_live_out(bb, live_out);
                compute_new_live_in(bb, std::move(live_out));
            }

        } while (changed);
    }


    std::unique_ptr<result_type> result() noexcept {
        std::unordered_map<const LIRBlock*, LiveInfo> liveness;
        for (auto& [bb, live_info] : m_liveness) {
            liveness.emplace(bb, LiveInfo(std::move(live_info.first), std::move(live_info.second)));
        }

        return std::make_unique<result_type>(std::move(liveness));
    }

    static LivenessAnalysis create(AnalysisPassManagerBase<LIRFuncData> *cache, const LIRFuncData *data) {
        auto ordering = cache->analyze<PreorderTraverseBase<LIRFuncData>>(data);
        return LivenessAnalysis(*ordering);
    }

private:
    /**
     * Performs: b.live_out = b.live_out ∪ new_live_out
     */
    bool add_all_live_out(const basic_block* bb, const LIRValSet& new_live_out) {
        auto& live_out_bb = m_liveness.at(bb).second;
        auto changed = false;
        for (const auto& succ: new_live_out) {
            changed |= live_out_bb.insert(succ).second;
        }

        return changed;
    }

    /**
    * Performs: live_in = (b.live_out – b.live_kill) ∪ b.live_gen
    */
    void compute_new_live_in(const basic_block* bb, LIRValSet&& new_live_out) {
        LIRValSet live_in(std::move(new_live_out));
        const auto& kill_gen = m_kill_gen_set.at(bb);

        auto predicate = [&](const LIRVal& reg) -> bool {
            return kill_gen.first.contains(reg);
        };
        std::erase_if(live_in, predicate);
        m_liveness.at(bb).first = std::move(live_in);
    }

    void compute_local_live_set() {
        for (const auto bb: m_ordering) {
            LIRValSet gen;
            LIRValSet kill;

            for (const auto& inst: bb->instructions()) {
                if (!inst.isa(parallel_copy())) {
                    for (auto in: inst.inputs()) {
                        const auto vreg = LIRVal::try_from(in);
                        if (!vreg.has_value()) {
                            continue;
                        }

                        if (kill.contains(vreg.value())) {
                            continue;
                        }

                        gen.insert(vreg.value());
                    }
                }

                for (const auto& out: LIRVal::try_from(&inst)) {
                    kill.insert(out);
                }
            }

            m_kill_gen_set.emplace(bb, std::pair(kill, gen));
        }
    }

    void setup_liveness() {
        for (const auto& bb: m_ordering) {
            m_liveness.emplace(bb, std::pair<LIRValSet, LIRValSet>{});
        }
    }

    const Ordering<LIRBlock>& m_ordering;
    std::unordered_map<const basic_block*, std::pair<LIRValSet, LIRValSet>> m_kill_gen_set{};
    std::unordered_map<const basic_block*, std::pair<LIRValSet, LIRValSet>> m_liveness{};
};
