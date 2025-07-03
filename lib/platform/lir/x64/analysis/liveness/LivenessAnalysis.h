#pragma once

#include "LiveInfo.h"
#include "pass/analysis/AnalysisPassCacheBase.h"
#include "platform/lir/x64/MachBlock.h"
#include "platform/lir/x64/Matcher.h"
#include "platform/lir/x64/ObjFuncData.h"
#include "platform/lir/x64/VRegMap.h"


class LivenessAnalysis final {
public:
    using result_type = LivenessAnalysisInfo;
    using basic_block = MachBlock;
    static constexpr auto analysis_kind = AnalysisType::LivenessAnalysis;

private:
    explicit LivenessAnalysis(const Ordering<MachBlock> &ordering):
        m_ordering(ordering) {}

public:
    void run() {
        compute_local_live_set();
        setup_liveness();

        auto changed = false;
        do {
            for (const auto& bb: m_ordering) {
                VRegSet live_out;
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
        std::unordered_map<const MachBlock*, LiveInfo> liveness;
        for (auto& [bb, live_info] : m_liveness) {
            liveness.emplace(bb, LiveInfo(std::move(live_info.first), std::move(live_info.second)));
        }

        return std::make_unique<result_type>(std::move(liveness));
    }

    static LivenessAnalysis create(AnalysisPassCacheBase<ObjFuncData> *cache, const ObjFuncData *data) {
        auto ordering = cache->analyze<LinearScanOrderBase<ObjFuncData>>(data);
        return LivenessAnalysis(*ordering);
    }

private:
    /**
     * Performs: b.live_out = b.live_out ∪ new_live_out
     */
    bool add_all_live_out(const basic_block* bb, const VRegSet& new_live_out) {
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
    void compute_new_live_in(const basic_block* bb, VRegSet&& new_live_out) {
        VRegSet live_in(std::move(new_live_out));
        const auto& kill_gen = m_kill_gen_set.at(bb);

        auto predicate = [&](const VReg& reg) -> bool {
            return kill_gen.first.contains(reg);
        };
        std::erase_if(live_in, predicate);
        m_liveness.at(bb).first = std::move(live_in);
    }

    void compute_local_live_set() {
        for (const auto bb: m_ordering) {
            VRegSet gen;
            VRegSet kill;

            for (const auto& inst: bb->instructions()) {
                if (!inst.isa(parallel_copy())) {
                    for (auto in: inst.inputs()) {
                        const auto vreg = VReg::from(in);
                        if (!vreg.has_value()) {
                            continue;
                        }

                        if (kill.contains(vreg.value())) {
                            continue;
                        }

                        gen.insert(vreg.value());
                    }
                }

                for (const auto& out: inst.outputs()) {
                    kill.insert(out);
                }
            }

            m_kill_gen_set.emplace(bb, std::pair(kill, gen));
        }
    }

    void setup_liveness() {
        for (const auto& bb: m_ordering) {
            m_liveness.emplace(bb, std::pair<VRegSet, VRegSet>{});
        }
    }

    const Ordering<MachBlock>& m_ordering;
    std::unordered_map<const basic_block*, std::pair<VRegSet, VRegSet>> m_kill_gen_set{};
    std::unordered_map<const basic_block*, std::pair<VRegSet, VRegSet>> m_liveness{};
};
