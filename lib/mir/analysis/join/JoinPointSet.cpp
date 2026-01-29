#include "JoinPointSet.h"

#include "mir/analysis/escape/EscapeAnalysis.h"
#include "base/analysis/dom/DominatorTreeEvalBase.h"
#include "mir/instruction/Alloc.h"
#include "mir/instruction/Store.h"

namespace {
    class AllocStoreAnalysis final {
    public:
        using Result = std::unordered_map<const Alloc *, std::unordered_set<const BasicBlock *>>;

        explicit AllocStoreAnalysis(const FunctionData *data, const EscapeAnalysisResult* escape_analysis) noexcept:
            m_data(data),
            m_escape_analysis(escape_analysis) {}

        Result run() {
            std::unordered_map<const Alloc *, std::unordered_set<const BasicBlock *>> alloc_info;

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

                        blocks.emplace(user->owner());
                    }
                    alloc_info.emplace(alloc, std::move(blocks));
                }
            }

            return alloc_info;
        }

    private:
        const FunctionData *m_data;
        const EscapeAnalysisResult* m_escape_analysis;
    };
}


void JoinPointSet::run() {
    const auto alloc_info = AllocStoreAnalysis(m_data, m_escape_analysis).run();
    for (const auto& [alloc, stores]: alloc_info) {

    }
}

void JoinPointSet::evaluate_joins(const Alloc *alloc, std::unordered_set<const BasicBlock *> &&stores) noexcept {
    std::unordered_set<const BasicBlock *> phi_places;

    const auto frontiers = m_dom_tree->dominators()
    while (!stores.empty()) {
        const auto x = *stores.begin();
        stores.erase(stores.begin());


    }

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
