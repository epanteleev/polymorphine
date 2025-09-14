#include "FunctionVisibility.h"

#include <utility>
#include <ostream>

std::ostream & operator<<(std::ostream &os, const FunctionVisibility &visibility) {
    switch (visibility) {
        case FunctionVisibility::EXTERN:   return os << "extern";
        case FunctionVisibility::INTERNAL: return os << "internal";
        case FunctionVisibility::DEFAULT:  return os;
        default: std::unreachable();
    }
}
