#pragma once

#include <unordered_map>
#include <optional>

#include "DominatorTreeNode.h"
#include "DominatorTreeNodeIterator.h"
#include "ImmediateDominatorIterator.h"
#include "base/analysis/AnalysisPass.h"
#include "base/Constrains.h"

template<CodeBlock BB>
class DominatorTree final: public AnalysisPassResult {
public:
    using dom_node = DominatorTreeNode<BB>;

    explicit DominatorTree(std::unordered_map<BB*, dom_node> &&dominator_tree) noexcept:
        m_dominator_tree(std::move(dominator_tree)) {}

    [[nodiscard]]
    bool dominates(BB* const dominator, BB* const target) {
        for (const auto dom: dominators(target)) {
            if (dom->m_me == dominator) {
                return true;
            }
        }

        return false;
    }

    /** @return strict dominators **/
    [[nodiscard]]
    Dominators<BB> dominators(BB* const target) const {
        const auto idom_node = m_dominator_tree.find(target);
        if (idom_node == m_dominator_tree.end()) {
            return Dominators<BB>(nullptr);
        }

        return Dominators(idom_node->second.idom);
    }

    /** @return immediate dominator of given block
     * or std::nullopt if the block doesn't have idom.
     * **/
    [[nodiscard]]
    std::optional<BB *> immediate_dominator(BB *const target) const {
        const auto idom_node = m_dominator_tree.find(target);
        if (idom_node == m_dominator_tree.end()) {
            return std::nullopt;
        }
        if (idom_node->second.idom == nullptr) {
            return std::nullopt;
        }

        return idom_node->second.idom->m_me;
    }

    [[nodiscard]]
    ImmediateDominators<BB> immediate_dominators() const noexcept {
        return ImmediateDominators(m_dominator_tree);
    }

private:
    std::unordered_map<BB*, dom_node> m_dominator_tree;
};