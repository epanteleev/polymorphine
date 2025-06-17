#pragma once

#include "Address.h"
#include "Immediate.h"
#include "Register.h"

namespace aasm {
    namespace opc {
        static constexpr std::uint8_t PUSH_M = 0xFF;
        static constexpr std::uint8_t MOV_RR = 0x89;
        static constexpr std::uint8_t MOV_RR_8 = 0x88;
        static constexpr std::uint8_t POP_M = 0x8F;
        static constexpr std::uint8_t POP_R = 0x58;
        static constexpr std::uint8_t RET = 0xC3;
        static constexpr std::uint8_t PUSH_R = 0x50;
    }

    class operand final {
    public:
        operand(const std::uint8_t size, int imm32):
            m_size(size),
            m_data(imm32) {}

        operand(const std::uint8_t size, const GPReg& reg):
            m_size(size),
            m_data(reg) {}

        operand(const std::uint8_t size, const Address& mem):
            m_size(size),
            m_data(mem) {}

        operand(const std::uint8_t size, const Imm& imm):
            m_size(size),
            m_data(imm) {}

        [[nodiscard]]
        const int& width() const {
            return std::get<int>(m_data);
        }

        [[nodiscard]]
        const GPReg& reg() const {
            return std::get<GPReg>(m_data);
        }

        [[nodiscard]]
        const Address& mem() const {
            return std::get<Address>(m_data);
        }

        [[nodiscard]]
        const Imm& imm() const {
            return std::get<Imm>(m_data);
        }

        std::uint8_t m_size;
        std::variant<
            int, /* Width of operand, 1, 2, 4 or 8. */
            GPReg, /* Register operand. */
            Address, /* Memory operand. */
            Imm /* Immediate operand. */
        > m_data;
    };

    class Instruction final {
    public:
        Instruction(std::uint8_t opcode, const operand& source, const operand& dest):
            opcode(opcode),
            source(source),
            dest(dest) {}

        std::uint8_t opcode;
        operand source;
        operand dest;
    };
}