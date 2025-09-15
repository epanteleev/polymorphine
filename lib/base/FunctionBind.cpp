#include "FunctionBind.h"

#include <utility>
#include <ostream>

std::ostream & operator<<(std::ostream &os, const FunctionBind &bind) {
    switch (bind) {
        case FunctionBind::EXTERN:   return os << "extern";
        case FunctionBind::INTERNAL: return os << "internal";
        case FunctionBind::DEFAULT:  return os;
        default: std::unreachable();
    }
}
