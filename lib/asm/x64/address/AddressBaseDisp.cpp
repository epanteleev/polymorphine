#include <ostream>
#include <optional>

#include "asm/x64/Relocation.h"
#include "asm/x64/reg/Register.h"
#include "asm/x64/Common.h"

#include "AddressBaseDisp.h"


namespace aasm {
    std::ostream& operator<<(std::ostream & os, const AddressBaseDisp & addr) {
        if (addr.m_displacement != 0) {
            os << addr.m_displacement;
        }
        os << "(%" << addr.m_base.name(8);
        return os << ')';
    }
}