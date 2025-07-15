#pragma once

#include <vector>
#include <iomanip>
#include <iosfwd>
#include <ranges>

#include "Register.h"
#include "Address.h"
#include "instruction/CPUInstruction.h"

namespace aasm {
    class Assembler final {
    public:
        constexpr void ret() {
            m_instructions.emplace_back(Ret());
        }

        constexpr void pop(const std::size_t size, const GPReg r) {
            m_instructions.emplace_back(PopR(size, r));
        }

        constexpr void pop(const std::size_t size, const Address& addr) {
            m_instructions.emplace_back(PopM(size, addr));
        }

        constexpr void push(const std::size_t size, const GPReg r) {
            m_instructions.emplace_back(PushR(size, r));
        }

        constexpr void push(const std::size_t size, const Address& addr) {
            m_instructions.emplace_back(PushM(size, addr));
        }

        constexpr void push(const std::size_t size, const std::int32_t value) {
            assertion(in_size_range(value, size), "Value {} is not in range for size {}", value, size);
            m_instructions.emplace_back(PushI(value, size));
        }

        constexpr void mov(const std::size_t size, const GPReg src, const GPReg dst) {
            m_instructions.emplace_back(MovRR(size, src, dst));
        }

        constexpr void mov(const std::size_t size, const std::int64_t src, const GPReg dst) {
            m_instructions.emplace_back(MovRI(size, src, dst));
        }

        constexpr void mov(const std::size_t size, const GPReg src, const Address& dst) {
            m_instructions.emplace_back(MovMR(size, src, dst));
        }

        constexpr void mov(const std::size_t size, const Address& src, GPReg dst) {
            m_instructions.emplace_back(MovRM(size, src, dst));
        }

        friend std::ostream &operator<<(std::ostream &os, const Assembler &assembler);

        template<CodeBuffer Buffer>
        constexpr void emit(Buffer& buffer) const {
            for (const auto& instruction: m_instructions) {
                instruction.emit(buffer);
            }
        }

        [[nodiscard]]
        constexpr std::size_t size() const noexcept {
            return m_instructions.size();
        }

    private:
        std::vector<X64Instruction> m_instructions;
    };

    inline std::ostream & operator<<(std::ostream &os, const Assembler &assembler) {
        const auto pretty_print = static_cast<int>(os.width());
        for (const auto& [idx, instruction]: std::ranges::enumerate_view(assembler.m_instructions)) {
            if (idx > 0) {
                os << std::endl;
            }
            if (pretty_print > 0) {
                os << std::setw(pretty_print) << ' ' << instruction;
            } else {
                os << instruction;
            }
        }

        return os;
    }
}
