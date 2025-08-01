#include "FunctionDeclaration.h"

#include <utility>
#include <ostream>

std::ostream & operator<<(std::ostream &os, const FunctionLinkage &linkage) {
    switch (linkage) {
        case FunctionLinkage::EXTERN: return os << "extern";
        case FunctionLinkage::INTERNAL: return os << "internal";
        default: std::unreachable();
    }
}

std::ostream & operator<<(std::ostream &os, const FunctionDeclaration &decl) {
    return os << decl.m_linkage << ' ' << decl.m_prototype;
}