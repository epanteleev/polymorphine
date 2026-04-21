#pragma once

#include "DominatorTreeNode.h"

template<CodeBlock BB>
class DominatorTreeNodeChildren final {
public:
    using value = std::span<const DominatorTreeNode<BB>>;
    using iterator = value::iterator;

    explicit DominatorTreeNodeChildren(value span) noexcept:
        m_span(span) {}

    [[nodiscard]]
    iterator begin() const noexcept {
        return m_span.begin();
    }

private:
    value m_span;
};

template<CodeBlock BB>
class DominatorTreeNodeChildrenIterator final {
public:
    using iterator = DominatorTreeNode<BB>::iterator;

private:
    iterator m_iterator;
};