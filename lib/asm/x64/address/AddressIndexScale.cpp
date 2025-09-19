#include <ostream>
#include <optional>

#include "asm/x64/Relocation.h"
#include "asm/x64/reg/Register.h"
#include "asm/x64/Common.h"

#include "AddressIndexScale.h"

namespace aasm {
    std::ostream& operator<<(std::ostream & os, const AddressIndexScale & addr) {
        if (addr.m_displacement != 0) {
            os << addr.m_displacement;
        }
        os << '(';
        os << '%' << addr.m_base.name(8);
        os << ',';

        os << '%' << addr.m_index.name(8);
        if (addr.scale != 1) {
            os << ',' << static_cast<std::int32_t>(addr.scale);
        }
        return os << ')';
    }
}