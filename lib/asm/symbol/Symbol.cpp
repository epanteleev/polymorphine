#include "Symbol.h"

#include <ostream>

namespace aasm {
    std::ostream & operator<<(std::ostream &os, const Symbol &symbol) {
        return os << symbol.m_name;
    }
}