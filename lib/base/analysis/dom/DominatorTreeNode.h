#pragma once

#include <vector>
#include "base/Constrains.h"

template<CodeBlock BB>
struct DominatorTreeNode final {
    explicit DominatorTreeNode(BB *const me) noexcept:
        m_me(me) {}

    DominatorTreeNode *idom{};
    BB *const m_me;
    std::vector<DominatorTreeNode *> children{};
};