#pragma once

#include <vector>

#include "Register.h"
#include "Address.h"
#include "AsmBuffer.h"
#include "instruction/CPUInstruction.h"

namespace aasm {
    class Assembler final {
    public:
        constexpr void ret() {
            m_instructions.emplace_back(Ret());
        }

        constexpr void pop(const std::uint8_t size, const GPReg r) {
            m_instructions.emplace_back(PopR(size, r));
        }

        constexpr void pop(const std::uint8_t size, const Address& addr) {
            m_instructions.emplace_back(PopM(size, addr));
        }

        constexpr void push(const std::uint8_t size, const GPReg r) {
            m_instructions.emplace_back(PushR(size, r));
        }

        constexpr void push(const std::uint8_t size, const Address& addr) {
            m_instructions.emplace_back(PushM(size, addr));
        }

        constexpr void push(const std::uint8_t size, const std::int32_t value) {
            m_instructions.emplace_back(PushI(value, size));
        }

        constexpr void mov(const std::uint8_t size, const GPReg src, const GPReg dst) {
            m_instructions.emplace_back(MovRR(size, src, dst));
        }

        constexpr void mov(const std::uint8_t size, const std::int64_t src, const GPReg dst) {
            m_instructions.emplace_back(MovRI(size, src, dst));
        }

        constexpr void mov(const std::uint8_t size, const GPReg src, const Address& dst) {
            m_instructions.emplace_back(MovMR(size, src, dst));
        }

        constexpr void mov(const std::uint8_t size, const Address& src, GPReg dst) {
            m_instructions.emplace_back(MovRM(size, src, dst));
        }

        constexpr void mov(const std::uint8_t size, const std::int32_t src, const Address& dst) {
            m_instructions.emplace_back(MovMI(size, src, dst));
        }

        constexpr void add(const std::uint8_t size, const GPReg src, const GPReg dst) {
            m_instructions.emplace_back(AddRR(size, src, dst));
        }

        constexpr void add(const std::uint8_t size, const std::int32_t src, const GPReg dst) {
            m_instructions.emplace_back(AddRI(size, src, dst));
        }

        constexpr void add(const std::uint8_t size, const Address& src, const GPReg dst) {
            m_instructions.emplace_back(AddRM(size, src, dst));
        }

        constexpr void cmp(const std::uint8_t size, const GPReg src, const GPReg dst) {
            m_instructions.emplace_back(CmpRR(size, src, dst));
        }

        constexpr void cmp(const std::uint8_t size, const std::int32_t imm, const GPReg dst) {
            m_instructions.emplace_back(CmpRI(size, imm, dst));
        }

        constexpr void cmp(const std::uint8_t size, const Address& src, const GPReg dst) {
            m_instructions.emplace_back(CmpRM(size, src, dst));
        }

        constexpr void cmp(const std::uint8_t size, const std::int32_t imm, const Address& src) {
            m_instructions.emplace_back(CmpMI(size, imm, src));
        }

        constexpr void cmp(const std::uint8_t size, const GPReg src, const Address& dst) {
            m_instructions.emplace_back(CmpMR(size, src, dst));
        }

        constexpr void setcc(const CondType type, const GPReg reg) {
            m_instructions.emplace_back(SetCCR(type, reg));
        }

        constexpr Label create_label() {
            const auto size = m_label_table.size();
            m_label_table.emplace_back(constants::NO_OFFSET);
            return Label(size);
        }

        constexpr void set_label(const Label& label) {
            if (m_label_table[label.id()] != constants::NO_OFFSET) {
                die("label already set: id=%u", label.id());
            }

            m_label_table[label.id()] = m_instructions.size();
        }

        constexpr void jmp(const Label& label) {
            m_instructions.emplace_back(Jmp(label));
        }

        constexpr void jcc(const CondType type, const Label& label) {
            m_instructions.emplace_back(Jcc(type, label));
        }

        [[nodiscard]]
        constexpr std::size_t size() const noexcept {
            return m_instructions.size();
        }

        constexpr AsmBuffer to_buffer() noexcept {
            return {std::move(m_label_table), std::move(m_instructions)};
        }

    private:
        std::vector<std::uint32_t> m_label_table; // HashMap from 'label' to instruction index
        std::vector<X64Instruction> m_instructions;
    };
}
