#pragma once

#include <memory>
#include <unordered_map>
#include <optional>

#include "DominatorTreeNode.h"
#include "DominatorTreeNodeIterator.h"
#include "ImmediateDominatorIterator.h"
#include "base/analysis/AnalysisPass.h"
#include "base/Constrains.h"

template<CodeBlock BB>
class DominatorTree final: public AnalysisPassResult {
    using dom_node_ptr = std::unique_ptr<DominatorTreeNode<BB>>;

public:
    using dom_node = DominatorTreeNode<BB>;

    explicit DominatorTree(std::unordered_map<BB*, dom_node_ptr> &&dominator_tree) noexcept:
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

        return Dominators(idom_node->second->idom);
    }

    [[nodiscard]]
    std::optional<BB *> immediate_dominator(BB *const target) const {
        const auto idom_node = m_dominator_tree.find(target);
        if (idom_node == m_dominator_tree.end()) {
            return std::nullopt;
        }

        return idom_node->second->idom->m_me;
    }

    [[nodiscard]]
    ImmediateDominators<BB> immediate_dominators() const noexcept {
        return ImmediateDominators(&m_dominator_tree);
    }

    [[nodiscard]]
    auto begin() const noexcept {
        return m_dominator_tree.begin();
    }

    [[nodiscard]]
    auto end() const noexcept {
        return m_dominator_tree.end();
    }

    template<typename Os>
    Os &print(Os &os) const {
        os << '[';
        for (auto& [k, v]: m_dominator_tree) {
            k->print_short_name(os);
            os << " -> ";
            if (v->idom) {
                v->idom->m_me->print_short_name(os);
            } else {
                os << "null";
            }
            os << " ";
        }
        return os << ']';
    }

private:
    std::unordered_map<BB*, dom_node_ptr> m_dominator_tree;
};