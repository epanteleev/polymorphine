#pragma once

#include <vector>

#include "reg/Register.h"
#include "address/Address.h"
#include "AsmBuffer.h"
#include "instruction/CPUInstruction.h"
#include "symbol/Symbol.h"

namespace aasm {
    class AsmEmitter final {
    public:
        constexpr void ret() {
            m_instructions.emplace_back(details::Ret());
        }

        constexpr void pop(const std::uint8_t size, const GPReg r) {
            m_instructions.emplace_back(details::PopR(size, r));
        }

        constexpr void pop(const std::uint8_t size, const Address& addr) {
            m_instructions.emplace_back(details::PopM(size, addr));
        }

        constexpr void push(const std::uint8_t size, const GPReg r) {
            m_instructions.emplace_back(details::PushR(size, r));
        }

        constexpr void push(const std::uint8_t size, const Address& addr) {
            m_instructions.emplace_back(details::PushM(size, addr));
        }

        constexpr void push(const std::uint8_t size, const std::int32_t value) {
            m_instructions.emplace_back(details::PushI(value, size));
        }

        constexpr void mov(const std::uint8_t size, const GPReg src, const GPReg dst) {
            m_instructions.emplace_back(details::MovRR(size, src, dst));
        }

        // Conditional Move
        constexpr void cmov(const std::uint8_t size, const CondType cond, const GPReg src, const GPReg dst) {
            m_instructions.emplace_back(details::CMovRR(size, src, dst, cond));
        }

        constexpr void cmov(const std::uint8_t size, const CondType cond, const Address& src, const GPReg dst) {
            m_instructions.emplace_back(details::CMovRM(size, src, dst, cond));
        }

        constexpr void mov(const std::uint8_t size, const std::int64_t src, const GPReg dst) {
            m_instructions.emplace_back(details::MovRI(size, src, dst));
        }

        constexpr void mov(const std::uint8_t size, const GPReg src, const Address& dst) {
            m_instructions.emplace_back(details::MovMR(size, src, dst));
        }

        constexpr void mov(const std::uint8_t size, const Address& src, GPReg dst) {
            m_instructions.emplace_back(details::MovRM(size, src, dst));
        }

        constexpr void mov(const std::uint8_t size, const std::int32_t src, const Address& dst) {
            m_instructions.emplace_back(details::MovMI(size, src, dst));
        }

        // Add
        constexpr void add(const std::uint8_t size, const GPReg src, const GPReg dst) {
            m_instructions.emplace_back(details::AddRR(size, src, dst));
        }

        constexpr void add(const std::uint8_t size, const std::int32_t src, const GPReg dst) {
            m_instructions.emplace_back(details::AddRI(size, src, dst));
        }

        constexpr void add(const std::uint8_t size, const Address& src, const GPReg dst) {
            m_instructions.emplace_back(details::AddRM(size, src, dst));
        }

        constexpr void add(const std::uint8_t size, const GPReg src, const Address& dst) {
            m_instructions.emplace_back(details::AddMR(size, src, dst));
        }

        constexpr void add(const std::uint8_t size, const std::int32_t src, const Address& dst) {
            m_instructions.emplace_back(details::AddMI(size, src, dst));
        }

        // Subtract
        constexpr void sub(const std::uint8_t size, const GPReg src, const GPReg dst) {
            m_instructions.emplace_back(details::SubRR(size, src, dst));
        }

        constexpr void sub(const std::uint8_t size, const std::int32_t src, const GPReg dst) {
            m_instructions.emplace_back(details::SubRI(size, src, dst));
        }

        constexpr void sub(const std::uint8_t size, const std::int32_t src, const Address& dst) {
            m_instructions.emplace_back(details::SubMI(size, src, dst));
        }

        constexpr void sub(const std::uint8_t size, const Address& src, const GPReg dst) {
            m_instructions.emplace_back(details::SubRM(size, src, dst));
        }

        constexpr void sub(const std::uint8_t size, const GPReg src, const Address& dst) {
            m_instructions.emplace_back(details::SubMR(size, src, dst));
        }

        constexpr void cmp(const std::uint8_t size, const GPReg src, const GPReg dst) {
            m_instructions.emplace_back(details::CmpRR(size, src, dst));
        }

        constexpr void cmp(const std::uint8_t size, const std::int32_t imm, const GPReg dst) {
            m_instructions.emplace_back(details::CmpRI(size, imm, dst));
        }

        constexpr void cmp(const std::uint8_t size, const Address& src, const GPReg dst) {
            m_instructions.emplace_back(details::CmpRM(size, src, dst));
        }

        constexpr void cmp(const std::uint8_t size, const std::int32_t imm, const Address& src) {
            m_instructions.emplace_back(details::CmpMI(size, imm, src));
        }

        constexpr void cmp(const std::uint8_t size, const GPReg src, const Address& dst) {
            m_instructions.emplace_back(details::CmpMR(size, src, dst));
        }

        constexpr void call(const Symbol* name) {
            m_instructions.emplace_back(details::Call(name));
        }

        constexpr void call(const Address& addr) {
            m_instructions.emplace_back(details::CallM(addr));
        }

        constexpr void leave() {
            m_instructions.emplace_back(details::Leave());
        }

        constexpr void setcc(const CondType type, const GPReg reg) {
            m_instructions.emplace_back(details::SetCCR(type, reg));
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
            m_instructions.emplace_back(details::Jmp(label));
        }

        constexpr void jcc(const CondType type, const Label& label) {
            m_instructions.emplace_back(details::Jcc(type, label));
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
