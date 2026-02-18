#include "JoinPointSet.h"

#include "base/analysis/dom/DominatorTreeEvalBase.h"
#include "mir/analysis/escape/EscapeAnalysis.h"

#include "mir/instruction/Alloc.h"
#include "mir/instruction/Store.h"
#include "mir/analysis/Analysis.h"

using AllocStoreAnalysisResult = std::unordered_map<const Alloc *, std::unordered_set<BasicBlock *>>;

static std::unordered_set<BasicBlock *> collect_blocks(const Alloc *alloc) {
    std::unordered_set<BasicBlock *> blocks;
    for (const auto& user: alloc->users()) {
        if (!user->isa(store())) {
            continue;
        }

        blocks.emplace(user->owner());
    }
    return blocks;
}

static AllocStoreAnalysisResult alloc_store_analysis(const FunctionData *data, const EscapeAnalysisResult* escape_analysis) {
    AllocStoreAnalysisResult alloc_info;
    for (const auto& bb: data->basic_blocks()) {
        for (const auto& inst: bb.instructions()) {
            if (!inst.isa(alloc())) {
                continue;
            }

            const auto alloc = Alloc::cast(&inst);
            if (escape_analysis->escape_state(alloc) != EscapeState::NOESCAPE) {
                continue;
            }

            alloc_info.emplace(alloc, collect_blocks(alloc));
        }
    }

    return alloc_info;
}

void JoinPointSet::run() {
    auto alloc_info = alloc_store_analysis(m_data, m_escape_analysis);
    for (auto& [alloc, stores]: alloc_info) {
        evaluate_joins(alloc, std::move(stores));
    }
}

void JoinPointSet::evaluate_joins(const Alloc *alloc, std::unordered_set<BasicBlock *> &&stores) noexcept {
    std::unordered_set<const BasicBlock *> phi_places;

    while (!stores.empty()) {
        const auto x = *stores.begin();
        stores.erase(stores.begin());

        for (const auto frontier: m_frontiers.frontiers(x)) {
            if (phi_places.contains(frontier)) {
                continue;
            }

            add_value(frontier, alloc);
            phi_places.emplace(frontier);

            if (!has_user_in_block(frontier, alloc)) {
                stores.emplace(frontier);
            }
        }
    }
}

void JoinPointSet::add_value(BasicBlock *const bb, const Alloc *alloc) noexcept {
    const auto allocs = m_join_set.find(bb);
    if (allocs != m_join_set.end()) {
        allocs->second.emplace(alloc);
        return;
    }

    std::unordered_set<const Alloc *> blocks; //TODO std::vector????
    blocks.emplace(alloc);
    m_join_set.emplace(bb, std::move(blocks));
}

JoinPointSet JoinPointSet::create(AnalysisPassManagerBase<FunctionData> *cache, const FunctionData *data) {
    const auto frontiers = cache->analyze<DominanceFrontiersEval>(data);
    const auto escape_analysis = cache->analyze<EscapeAnalysis>(data);
    return JoinPointSet(*frontiers, escape_analysis, data);
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