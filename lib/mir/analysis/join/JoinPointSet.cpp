#include "JoinPointSet.h"

#include "mir/analysis/escape/EscapeAnalysis.h"
#include "base/analysis/dom/DominatorTreeEvalBase.h"
#include "mir/instruction/Alloc.h"
#include "mir/instruction/Store.h"

namespace {
    class AllocStoreAnalysis final {
    public:
        explicit AllocStoreAnalysis(const FunctionData *data, const EscapeAnalysisResult* escape_analysis) noexcept:
            m_data(data),
            m_escape_analysis(escape_analysis) {}

        void run() {
            for (const auto& bb: m_data->basic_blocks()) {
                for (const auto& inst: bb.instructions()) {
                    if (!inst.isa(alloc())) {
                        continue;
                    }

                    const auto alloc = Alloc::cast(&inst);
                    if (m_escape_analysis->escape_state(alloc) != EscapeState::NOESCAPE) {
                        continue;
                    }

                    std::unordered_set<const BasicBlock *> blocks;
                    for (const auto& user: alloc->users()) {
                        if (!user->isa(store())) {
                            continue;
                        }

                        //m_alloc_info.emplace(
                    }
                }
            }
        }

    private:
        const FunctionData *m_data;
        const EscapeAnalysisResult* m_escape_analysis;
        std::unordered_map<const Alloc *, std::unordered_set<const BasicBlock *>> m_alloc_info;
    };
}


void JoinPointSet::run() {

}

JoinPointSet JoinPointSet::create(AnalysisPassManagerBase<FunctionData> *cache, const FunctionData *data) {
    const auto dom = cache->analyze<DominatorTreeEvalBase<FunctionData>>(data);
    const auto escape_analysis = cache->analyze<EscapeAnalysis>(data);
    return JoinPointSet(dom, escape_analysis, data);
}

bool JoinPointSet::has_user_in_block(const BasicBlock *block, const Alloc *alloc) noexcept {
    if (alloc->owner() == block) {
        return true;
    }

    for (auto& user: alloc->users()) {
        if (!user->isa(store())) {
            continue;
        }

        if (user->owner() == block) {
            return true;
        }
    }

    return false;
}
