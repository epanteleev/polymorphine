#pragma once

#include <cstdint>
#include <iosfwd>
#include <span>
#include <vector>

#include "lir/x64/lir_frwd.h"

/**
 * Represents an argument in the Low-Level Intermediate Representation (LIR).
 * This class encapsulates the index and size of the argument.
 */
class LIRArg final {
public:
    explicit LIRArg(const std::uint32_t index, const std::uint8_t size) noexcept:
        m_index(index), m_size(size) {}

    [[nodiscard]]
    std::uint8_t size() const noexcept { return m_size; }

    [[nodiscard]]
    std::uint32_t index() const noexcept { return m_index; }

    void add_user(LIRInstructionBase *inst) noexcept {
        m_used_in.push_back(inst);
    }

    void kill_user(LIRInstructionBase *inst) noexcept {
        std::erase(m_used_in, inst);
    }

    [[nodiscard]]
    std::span<LIRInstructionBase * const> users() const noexcept {
        return m_used_in;
    }

    friend std::ostream& operator<<(std::ostream& os, const LIRArg& op) noexcept;

private:
    const std::uint32_t m_index;
    const std::uint8_t m_size;
    std::vector<LIRInstructionBase *> m_used_in;
};

std::ostream& operator<<(std::ostream& os, const LIRArg& op) noexcept;