#pragma once
#include "platform/lir/x64/Vreg.h"

class VregBuilder final {
public:
    VReg mk_vreg(std::uint32_t bb_idx, const LIRArg* def) {
        return VReg::reg(m_inc++, bb_idx, const_cast<LIRArg *>(def));
    }

    VReg mk_vreg(std::uint32_t size, std::uint32_t bb_idx, LIRInstruction* def) {
        return VReg::reg(size, m_inc++, bb_idx, def);
    }

private:
    std::uint32_t m_inc{};
};
