#pragma once

#include <memory>
#include <ostream>
#include <unordered_map>
#include <vector>
#include <optional>

#include "base/analysis/AnalysisPass.h"
#include "base/Constrains.h"

template<CodeBlock BB>
class DominanceFrontiers final {
public:
    using frontier_set = std::vector<const BB*>;
    using frontier_map = std::unordered_map<const BB*, frontier_set>;

    explicit DominanceFrontiers(frontier_map &&frontiers) noexcept:
        m_frontiers(std::move(frontiers)) {}

    [[nodiscard]]
    std::optional<std::span<const BB* const>> frontiers(const BB* bb) const {
        const auto& frontiers = m_frontiers.find(bb);
        if (frontiers == m_frontiers.end()) {
            return std::nullopt;
        }

        return frontiers->second;
    }

private:
    frontier_map m_frontiers;
};


template<CodeBlock BB>
struct DominatorTreeNode final {
    explicit DominatorTreeNode(const BB *me):
        m_me(me) {}

    DominatorTreeNode *idom{};
    const BB *m_me;
    std::vector<DominatorTreeNode *> children{};
};

template<CodeBlock BB>
class DominatorTree final: public AnalysisPassResult {
    class DominatorTreeNodeIterator final {
    public:
        explicit DominatorTreeNodeIterator(DominatorTreeNode<BB> *node):
            m_dominator_node(node) {}

        DominatorTreeNodeIterator &operator++() {
            m_dominator_node = m_dominator_node->idom;
            return *this;
        }

        bool operator==(const DominatorTreeNodeIterator &other) const {
            return m_dominator_node == other.m_dominator_node;
        }

        bool operator!=(const DominatorTreeNodeIterator &other) const {
            return m_dominator_node != other.m_dominator_node;
        }

        DominatorTreeNode<BB>* operator->() const {
            return m_dominator_node;
        }

        DominatorTreeNode<BB>* operator*() const {
            return m_dominator_node;
        }

    private:
        DominatorTreeNode<BB>* m_dominator_node;
    };

    class Dominators final {
    public:
        explicit Dominators(DominatorTreeNode<BB> *node):
            m_dominator_node(node) {}

        DominatorTreeNodeIterator begin() const {
            return DominatorTreeNodeIterator(m_dominator_node);
        }

        DominatorTreeNodeIterator end() const {
            return DominatorTreeNodeIterator(nullptr);
        }

    private:
        DominatorTreeNode<BB> *m_dominator_node;
    };

public:
    using dom_node = std::unique_ptr<DominatorTreeNode<BB>>;
    using frontier_set = DominanceFrontiers<BB>::frontier_set;
    using frontier_map = DominanceFrontiers<BB>::frontier_map;

    explicit DominatorTree(std::unordered_map<const BB*, dom_node> &&dominator_tree) noexcept:
        dominator_tree(std::move(dominator_tree)) {}

    [[nodiscard]]
    bool dominates(const BB* dominator, const BB* target) {
        for (const auto dom: dominators(target)) {
            if (dom->m_me == dominator) {
                return true;
            }
        }

        return false;
    }

    /** @return strict dominators **/
    [[nodiscard]]
    Dominators dominators(const BB* target) const {
        return Dominators(dominator_tree.at(const_cast<BB*>(target)).get()->idom);
    }

    [[nodiscard]]
    DominanceFrontiers<BB> dominance_frontiers() const {
        frontier_map frontiers;
        frontiers.reserve(dominator_tree.size());
        for (const auto& [block, sub_tree]: dominator_tree) {
            if (sub_tree == nullptr) {
                continue;
            }

            frontiers.emplace(block, frontier_set{});
        }

        for (const auto& [block, node]: dominator_tree) {
            const auto preds = block->predecessors();
            if (preds.size() < 2) {
                continue;
            }

            const auto idom = node->idom != nullptr ? node->idom->m_me : nullptr;
            for (const auto& pred: preds) {
                auto runner = pred;
                while (runner && runner != idom) {
                    auto it = frontiers.find(runner);
                    if (it != frontiers.end()) {
                        it->second.push_back(block);
                    }

                    const auto dom_it = dominator_tree.find(runner);
                    if (dom_it == dominator_tree.end()) {
                        break;
                    }

                    const auto runner_node = dom_it->second.get();
                    runner = runner_node->idom != nullptr ? runner_node->idom->m_me : nullptr;
                }
            }
        }

        return DominanceFrontiers<BB>(std::move(frontiers));
    }

    std::ostream &print(std::ostream &os) const {
        os << '[';
        for (auto& [k, v]: dominator_tree) {
            k->print_short_name(os);
            os << " -> ";
            if (v->idom) {
                v->idom->m_me->print_short_name(os);
            } else {
                os << "null";
            }
            os << " ";
        }
        os << ']';
        return os;
    }

private:
    std::unordered_map<const BB*, dom_node> dominator_tree;
};
