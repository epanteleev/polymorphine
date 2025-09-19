#include <ostream>
#include <optional>

#include "asm/x64/Relocation.h"
#include "asm/x64/reg/Register.h"
#include "asm/x64/Common.h"

#include "AddressLiteral.h"

namespace aasm {
    std::ostream &operator<<(std::ostream &os, const AddressLiteral &address) {
        return os << '$' << address.m_symbol->name() << (address.m_displacement >= 0 ? "+" : "") << address.m_displacement;
    }
}