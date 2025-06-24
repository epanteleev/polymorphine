#pragma once

#include <cstdint>
#include <expected>

#include "mach_frwd.h"
#include "utility/Error.h"

class VReg final {
    enum class Kind : std::uint8_t {
        Reg,
        Addr
    };

    VReg(std::uint8_t size, Kind kind, std::uint32_t index, LIRInstruction* def):
        m_size(size),
        m_kind(kind),
        m_index(index),
        m_def(def) {}

public:
    static VReg addr(std::uint8_t size, std::uint32_t index, LIRInstruction* def) noexcept {
        return {size, Kind::Addr, index, def};
    }

    static VReg reg(std::uint8_t size, std::uint32_t index, LIRInstruction* def) noexcept {
        return {size, Kind::Reg, index, def};
    }

    static std::expected<VReg, Error> from(const LIROperand& op);

private:
    std::uint8_t m_size;
    Kind m_kind;
    std::uint32_t m_index;
    LIRInstruction* m_def;
};
