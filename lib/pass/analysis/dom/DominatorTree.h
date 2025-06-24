#pragma once

#include <memory>
#include <ostream>
#include <unordered_map>
#include <vector>

#include "pass/analysis/AnalysisPass.h"


template<CodeBlock BB>
struct DominatorNode final {
    explicit DominatorNode(BB *me):
        m_me(me) {}

    DominatorNode *idom{};
    BB *m_me;
    std::vector<DominatorNode *> children{};
};

template<CodeBlock BB>
class DominatorTree final: public AnalysisPassResult {
public:
    using dom_node = std::unique_ptr<DominatorNode<BB>>;

    explicit DominatorTree(std::unordered_map<BB*, dom_node> &&dominator_tree) noexcept
        : dominator_tree(std::move(dominator_tree)) {}

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
    std::unordered_map<BB*, dom_node> dominator_tree;
};
