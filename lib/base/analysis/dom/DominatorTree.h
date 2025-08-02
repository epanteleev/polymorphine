#pragma once

#include <memory>
#include <ostream>
#include <unordered_map>
#include <vector>

#include "base/analysis/AnalysisPass.h"
#include "base/Constrains.h"

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

    explicit DominatorTree(std::unordered_map<const BB*, dom_node> &&dominator_tree) noexcept
        : dominator_tree(std::move(dominator_tree)) {}

    bool dominates(const BB* dominator, const BB* target) {
        for (auto dom: dominators(target)) {
            if (dom->m_me == dominator) {
                return true;
            }
        }

        return false;
    }

    /** @return strict dominators **/
    Dominators dominators(const BB* target) const {
        return Dominators(dominator_tree.at(const_cast<BB*>(target)).get()->idom);
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
