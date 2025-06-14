#include <iostream>

#include "DominatorTree.h"

#include "BasicBlock.h"

void DominatorTree::print(std::ostream &os) const {
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
}
