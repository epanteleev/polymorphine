#pragma once
#include <memory>
#include <unordered_map>

#include "DominatorTreeNode.h"


template<CodeBlock BB>
class ImmediateDominatorIterator final {
    using dom_node_ptr = std::unique_ptr<DominatorTreeNode<BB>>;
    using idom_iterator = std::unordered_map<BB*, dom_node_ptr>::const_iterator;

public:
    explicit ImmediateDominatorIterator(idom_iterator dominator_tree) noexcept:
        m_block_idom(dominator_tree) {}

    ImmediateDominatorIterator &operator++() {
        ++m_block_idom;
        return *this;
    }

    bool operator==(const ImmediateDominatorIterator &other) const {
        return m_block_idom == other.m_block_idom;
    }

    bool operator!=(const ImmediateDominatorIterator &other) const {
        return m_block_idom != other.m_block_idom;
    }

    std::pair<BB* const, BB* const> operator->() const {
        return std::make_pair(m_block_idom->first, m_block_idom->second->idom->m_me);
    }

    std::pair<BB*, BB*> operator*() const {
        return std::make_pair(m_block_idom->first, m_block_idom->second->idom->m_me);
    }

private:
    idom_iterator m_block_idom;
};

template<CodeBlock BB>
class ImmediateDominators final {
    using dom_node_ptr = std::unique_ptr<DominatorTreeNode<BB>>;
    using idom_map = std::unordered_map<BB*, dom_node_ptr>;

public:
    explicit ImmediateDominators(const idom_map* node) noexcept:
        m_dom_map(node) {}

    ImmediateDominatorIterator<BB> begin() const {
        return ImmediateDominatorIterator<BB>(m_dom_map->begin());
    }

    ImmediateDominatorIterator<BB> end() const {
        return ImmediateDominatorIterator<BB>(m_dom_map->end());
    }

private:
    const idom_map *m_dom_map;
};