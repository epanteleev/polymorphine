#include <ostream>
#include <optional>

#include "utility/ArithmeticUtils.h"
#include "asm/x64/Relocation.h"
#include "asm/x64/reg/GPReg.h"
#include "asm/x64/Common.h"

#include "AddressLiteral.h"

namespace aasm {
    std::ostream &operator<<(std::ostream &os, const AddressLiteral &address) {
        os << '$' << address.m_symbol->name();
        if (address.m_displacement != 0) {
            os << '+' << address.m_displacement;
        }

        return os;
    }
}