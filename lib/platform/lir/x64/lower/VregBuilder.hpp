#pragma once
#include "platform/lir/x64/Vreg.h"

class VregBuilder final {
public:
    VReg mk_vreg(const LIRArg* def) {
        return VReg::reg(m_inc++, const_cast<LIRArg *>(def));
    }

    VReg mk_vreg(std::uint32_t size, LIRInstruction* def) {
        return VReg::reg(size, m_inc++, def);
    }

private:
    std::uint32_t m_inc{};
};
