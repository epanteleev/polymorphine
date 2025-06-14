#include "Ordering.h"

#include "BasicBlock.h"

std::ostream& operator<<(std::ostream &os, const Ordering& ordering) {
    os << '[';
    for (const auto &bb : ordering) {
        bb->print_short_name(os);
        os << ' ';
    }
    os << ']';
    return os;
}
