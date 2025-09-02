#include "FunctionLinkage.h"

#include <utility>
#include <ostream>

std::ostream & operator<<(std::ostream &os, const FunctionLinkage &linkage) {
    switch (linkage) {
        case FunctionLinkage::EXTERN:   return os << "extern";
        case FunctionLinkage::INTERNAL: return os << "internal";
        case FunctionLinkage::DEFAULT:  return os;
        default: std::unreachable();
    }
}
