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
        compute_global_live_set();
    }

    std::unique_ptr<result_type> result() noexcept {
        std::unordered_map<const LIRBlock*, LiveInfo> liveness;
        liveness.reserve(m_liveness.size());
        for (auto& [bb, live_info] : m_liveness) {
            liveness.emplace(bb, LiveInfo(std::move(live_info.first), std::move(live_info.second)));
        }

        return std::make_unique<result_type>(std::move(liveness));
    }

    static LivenessAnalysis create(AnalysisPassManagerBase<LIRFuncData> *cache, const LIRFuncData *data) {
        const auto ordering = cache->analyze<PreorderTraverseBase<LIRFuncData>>(data);
        return LivenessAnalysis(*ordering);
    }

private:
    void compute_local_live_set() {
        for (const auto bb: m_ordering) {
            LIRValSet gen;
            LIRValSet kill;

            for (const auto& inst: bb->instructions()) {
                if (!inst.isa(parallel_copy())) {
                    for (auto& in: inst.inputs()) {
                        const auto lir_val = LIRVal::try_from(in);
                        if (!lir_val.has_value()) {
                            continue;
                        }

                        if (kill.contains(lir_val.value())) {
                            continue;
                        }

                        gen.insert(lir_val.value());
                    }
                }

                for (const auto& out: LIRVal::defs(&inst)) {
                    kill.insert(out);
                }
            }

            m_kill_gen_set.emplace(bb, std::pair(kill, gen));
        }
    }

    void compute_global_live_set() {
        auto changed = false;
        do {
            changed = false;
            for (const auto& bb: std::ranges::reverse_view(m_ordering)) {
                auto& [live_in, live_out] = m_liveness.at(bb);

                for (const auto succ: bb->successors()) {
                    // live_out = b.live_out ∪ succ.live_in
                    for (const auto& reg: m_liveness.at(succ).first) {
                        changed |= live_out.insert(reg).second;
                    }
                }

                // union = (b.old_live_in ∪ b.old_live_out
                // live_in = (union – b.live_kill) ∪ b.live_gen
                for (const auto live_out_v: live_out) {
                    live_in.insert(live_out_v);
                }

                auto& [kill, gen] = m_kill_gen_set.at(bb);
                for (const auto& kill_reg: kill) {
                    live_in.erase(kill_reg);
                }

                for (const auto& gen_reg: gen) {
                    live_in.insert(gen_reg);
                }
            }
        } while (changed);
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