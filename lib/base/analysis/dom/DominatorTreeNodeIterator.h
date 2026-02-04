#pragma once
#include "DominatorTreeNode.h"

template<CodeBlock BB>
class DominatorTreeNodeIterator final {
public:
    explicit DominatorTreeNodeIterator(DominatorTreeNode<BB> *node) noexcept:
        m_dominator_node(node) {}

    DominatorTreeNodeIterator &operator++() {
        m_dominator_node = m_dominator_node->idom;
        return *this;
    }

    bool operator==(const DominatorTreeNodeIterator &other) const noexcept {
        return m_dominator_node == other.m_dominator_node;
    }

    bool operator!=(const DominatorTreeNodeIterator &other) const noexcept {
        return m_dominator_node != other.m_dominator_node;
    }

    DominatorTreeNode<BB>* operator->() const noexcept {
        return m_dominator_node;
    }

    DominatorTreeNode<BB>* operator*() const noexcept {
        return m_dominator_node;
    }

private:
    DominatorTreeNode<BB>* m_dominator_node;
};

template<CodeBlock BB>
class Dominators final {
public:
    explicit Dominators(DominatorTreeNode<BB> *node) noexcept:
        m_dominator_node(node) {}

    DominatorTreeNodeIterator<BB> begin() const noexcept {
        return DominatorTreeNodeIterator<BB>(m_dominator_node);
    }

    DominatorTreeNodeIterator<BB> end() const noexcept {
        return DominatorTreeNodeIterator<BB>(nullptr);
    }

private:
    DominatorTreeNode<BB> *m_dominator_node;
};