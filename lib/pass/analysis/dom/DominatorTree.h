#pragma once

#include <memory>
#include <ostream>
#include <unordered_map>
#include <vector>

#include "pass/analysis/AnalysisPass.h"


template<CodeBlock BB>
struct DominatorNode final {
    explicit DominatorNode(const BB *me):
        m_me(me) {}

    DominatorNode *idom{};
    const BB *m_me;
    std::vector<DominatorNode *> children{};
};

template<CodeBlock BB>
class DominatorTree final: public AnalysisPassResult {
    class DominatorNodeIterator final {
    public:
        explicit DominatorNodeIterator(DominatorNode<BB> *node):
            m_dominator_node(node) {}

        DominatorNodeIterator &operator++() {
            m_dominator_node = m_dominator_node->idom;
            return *this;
        }

        bool operator==(const DominatorNodeIterator &other) const {
            return m_dominator_node == other.m_dominator_node;
        }

        bool operator!=(const DominatorNodeIterator &other) const {
            return m_dominator_node != other.m_dominator_node;
        }

        DominatorNode<BB>* operator->() const {
            return m_dominator_node;
        }

        DominatorNode<BB>* operator*() const {
            return m_dominator_node;
        }

    private:
        DominatorNode<BB>* m_dominator_node;
    };

    class Dominators final {
    public:
        explicit Dominators(DominatorNode<BB> *node):
            m_dominator_node(node) {}

        DominatorNodeIterator begin() const {
            return DominatorNodeIterator(m_dominator_node);
        }

        DominatorNodeIterator end() const {
            return DominatorNodeIterator(nullptr);
        }

    private:
        DominatorNode<BB> *m_dominator_node;
    };


public:
    using dom_node = std::unique_ptr<DominatorNode<BB>>;

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
