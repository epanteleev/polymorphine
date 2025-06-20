#pragma once

#include <vector>
#include <iosfwd>
#include <unordered_map>
#include <memory>

#include "pass/analysis/AnalysisPass.h"


struct DominatorNode final {
    explicit DominatorNode(BasicBlock *me):
        m_me(me) {}

    DominatorNode *idom{};
    BasicBlock *m_me;
    std::vector<DominatorNode *> children{};
};

class DominatorTree final: public AnalysisPassResult {
public:
    explicit DominatorTree(std::unordered_map<BasicBlock*, std::unique_ptr<DominatorNode>> &&dominator_tree) noexcept
        : dominator_tree(std::move(dominator_tree)) {}


    std::ostream &print(std::ostream &os) const;

private:
    std::unordered_map<BasicBlock*, std::unique_ptr<DominatorNode>> dominator_tree;
};
