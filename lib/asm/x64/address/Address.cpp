#include "Address.h"

#include <ostream>

namespace aasm {
    std::ostream& operator<<(std::ostream & os, const Address & addr) {
        const auto visit = [&](const auto& address) {
            os << address;
        };
        std::visit(visit, addr.m_address);
        return os;
    }
}