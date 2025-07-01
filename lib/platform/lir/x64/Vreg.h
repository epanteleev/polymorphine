#pragma once

#include <cstdint>
#include <expected>

#include "LIRArg.h"
#include "mach_frwd.h"
#include "utility/Error.h"

class VReg final {
    enum class Op: std::uint8_t {
        Arg,
        Inst
    };

    VReg(std::uint8_t size, std::uint32_t index, std::uint32_t bb_idx, LIRArg *def):
        m_size(size),
        m_index(index),
        m_bb_idx(bb_idx),
        m_type(Op::Arg) {
        m_variant.m_arg = def;
    }

    VReg(std::uint8_t size, std::uint32_t index, std::uint32_t bb_idx, LIRInstruction *def):
        m_size(size),
        m_index(index),
        m_bb_idx(bb_idx),
        m_type(Op::Inst) {
        m_variant.m_inst = def;
    }

public:
    [[nodiscard]]
    std::optional<LIRArg*> arg() const noexcept {
        if (m_type == Op::Arg) {
            return m_variant.m_arg;
        }

        return std::nullopt;
    }

    [[nodiscard]]
    std::optional<LIRInstruction*> inst() const noexcept {
        if (m_type == Op::Inst) {
            return m_variant.m_inst;
        }

        return std::nullopt;
    }

    [[nodiscard]]
    std::uint8_t size() const noexcept {
        return m_size;
    }

    static VReg reg(std::uint32_t index, std::uint32_t bb_idx, LIRArg* def) noexcept {
        return {def->size(), index, bb_idx, def};
    }

    static VReg reg(std::uint8_t size, std::uint32_t index, std::uint32_t bb_idx, LIRInstruction* def) noexcept {
        return {size, index, bb_idx, def};
    }

    static std::expected<VReg, Error> from(const LIROperand& op);

    friend std::ostream& operator<<(std::ostream& os, const VReg& op) noexcept;

private:
    std::uint8_t m_size;
    std::uint32_t m_index;
    std::uint32_t m_bb_idx;
    Op m_type;
    union {
        LIRArg* m_arg;
        LIRInstruction* m_inst;
    } m_variant{};
};

std::ostream& operator<<(std::ostream& os, const VReg& op) noexcept;