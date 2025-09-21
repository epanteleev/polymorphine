#include "GlobalSymbol.h"
#include <ostream>

std::ostream& operator<<(std::ostream& os, const GlobalSymbol& sym) {
    return os << '$' << sym.name();
}