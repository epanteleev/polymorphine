#pragma once

#include <vector>
#include "base/Constrains.h"

template<CodeBlock BB>
class DominatorTreeNode final {
public:
    using iterator = std::vector<DominatorTreeNode *>::iterator;

    explicit DominatorTreeNode(BB *const me) noexcept:
        m_me(me) {}

    DominatorTreeNode *idom{};
    BB *const m_me;
    std::vector<DominatorTreeNode *> children{};
};