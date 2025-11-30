#pragma once

#include <vector>

#include "reg/GPReg.h"
#include "address/Address.h"
#include "AsmBuffer.h"
#include "instruction/X64Instruction.h"
#include "asm/symbol/Symbol.h"

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

        constexpr void cmp(const std::uint8_t size, const std::int32_t imm, const Address& dst) {
            m_instructions.emplace_back(details::CmpMI(size, imm, dst));
        }

        constexpr void cmp(const std::uint8_t size, const GPReg src, const Address& dst) {
            m_instructions.emplace_back(details::CmpMR(size, src, dst));
        }

        // Logical Exclusive OR
        constexpr void xxor(const std::uint8_t size, const GPReg src, const GPReg dst) {
            m_instructions.emplace_back(details::XorRR(size, src, dst));
        }

        constexpr void xxor(const std::uint8_t size, const std::int32_t src, const GPReg dst) {
            m_instructions.emplace_back(details::XorRI(size, src, dst));
        }

        constexpr void xxor(const std::uint8_t size, const Address& src, const GPReg dst) {
            m_instructions.emplace_back(details::XorRM(size, src, dst));
        }

        constexpr void xxor(const std::uint8_t size, const GPReg src, const Address& dst) {
            m_instructions.emplace_back(details::XorMR(size, src, dst));
        }

        constexpr void xxor(const std::uint8_t size, const std::int32_t src, const Address& dst) {
            m_instructions.emplace_back(details::XorMI(size, src, dst));
        }

        // Call
        constexpr void call(const Symbol* name) {
            m_instructions.emplace_back(details::Call(name));
        }

        constexpr void call(const Address& addr) {
            m_instructions.emplace_back(details::CallM(addr));
        }

        // Move With Zero-Extend
        constexpr void movzx(const std::uint8_t src_size, const std::uint8_t dst_size, const GPReg src, const GPReg dst) {
            m_instructions.emplace_back(details::MovzxRR(src_size, dst_size, src, dst));
        }

        constexpr void movzx(const std::uint8_t src_size, const std::uint8_t dst_size, const Address& src, const GPReg dst) {
            m_instructions.emplace_back(details::MovzxRM(src_size, dst_size, src, dst));
        }

        // Move With Zero-Extend
        constexpr void movsx(const std::uint8_t src_size, const std::uint8_t dst_size, const GPReg src, const GPReg dst) {
            m_instructions.emplace_back(details::MovsxRR(src_size, dst_size, src, dst));
        }

        constexpr void movsx(const std::uint8_t src_size, const std::uint8_t dst_size, const Address& src, const GPReg dst) {
            m_instructions.emplace_back(details::MovsxRM(src_size, dst_size, src, dst));
        }

        constexpr void movsxd(const std::uint8_t src_size, const std::uint8_t dst_size, const GPReg src, const GPReg dst) {
            m_instructions.emplace_back(details::MovsxdRR(src_size, dst_size, src, dst));
        }

        constexpr void movsxd(const std::uint8_t src_size, const std::uint8_t dst_size, const Address& src, const GPReg dst) {
            m_instructions.emplace_back(details::MovsxdRM(src_size, dst_size, src, dst));
        }

        // Two's Complement Negation
        constexpr void neg(const std::uint8_t size, const GPReg r) {
            m_instructions.emplace_back(details::NegR(size, r));
        }

        constexpr void neg(const std::uint8_t size, const Address& addr) {
            m_instructions.emplace_back(details::NegM(size, addr));
        }

        // IDIV — Signed Divide
        constexpr void idiv(const std::uint8_t size, const GPReg divisor) {
            m_instructions.emplace_back(details::IdivR(size, divisor));
        }

        constexpr void idiv(const std::uint8_t size, const Address& addr) {
            m_instructions.emplace_back(details::IdivM(size, addr));
        }

        // DIV — Unsigned Divide
        constexpr void div(const std::uint8_t size, const GPReg divisor) {
            m_instructions.emplace_back(details::UDivR(size, divisor));
        }

        constexpr void div(const std::uint8_t size, const Address& addr) {
            m_instructions.emplace_back(details::UDivM(size, addr));
        }

        // CWD/CDQ/CQO — Convert Word to Doubleword/Convert Doubleword to Quadword
        constexpr void cdq(const std::uint8_t size) {
            m_instructions.emplace_back(details::Cdq(size));
        }

        // Leave
        constexpr void leave() {
            m_instructions.emplace_back(details::Leave());
        }

        constexpr void setcc(const CondType type, const GPReg reg) {
            m_instructions.emplace_back(details::SetCCR(type, reg));
        }

        // Load Effective Address
        constexpr void lea(const Address& src, const GPReg dst) {
            m_instructions.emplace_back(details::Lea(dst, src));
        }

        // Move or Merge Scalar Single Precision Floating-Point Value
        constexpr void movss(const XmmReg src, const XmmReg dst) {
            m_instructions.emplace_back(details::MovssRR(src, dst));
        }

        constexpr void movss(const Address& src, const XmmReg dst) {
            m_instructions.emplace_back(details::MovssRM(src, dst));
        }

        constexpr void movss(const XmmReg src, const Address& dst) {
            m_instructions.emplace_back(details::MovssMR(src, dst));
        }

        // Move or Merge Scalar Double Precision Floating-Point Value
        constexpr void movsd(const XmmReg src, const XmmReg dst) {
            m_instructions.emplace_back(details::MovsdRR(src, dst));
        }

        constexpr void movsd(const Address& src, const XmmReg dst) {
            m_instructions.emplace_back(details::MovsdRM(src, dst));
        }

        constexpr void movsd(const XmmReg src, const Address& dst) {
            m_instructions.emplace_back(details::MovsdMR(src, dst));
        }

        // Add Scalar Single Precision Floating-Point Values
        constexpr void addss(const XmmReg src, const XmmReg dst) {
            m_instructions.emplace_back(details::AddssRR(src, dst));
        }

        constexpr void addss(const Address& src, const XmmReg dst) {
            m_instructions.emplace_back(details::AddssRM(src, dst));
        }

        // Add Scalar Double Precision Floating-Point Values
        constexpr void addsd(const XmmReg src, const XmmReg dst) {
            m_instructions.emplace_back(details::AddsdRR(src, dst));
        }

        constexpr void addsd(const Address& src, const XmmReg dst) {
            m_instructions.emplace_back(details::AddsdRM(src, dst));
        }

        // Subtract Scalar Single Precision Floating-Point Value
        constexpr void subss(const XmmReg src, const XmmReg dst) {
            m_instructions.emplace_back(details::SubssRR(src, dst));
        }

        constexpr void subss(const Address& src, const XmmReg dst) {
            m_instructions.emplace_back(details::SubssRM(src, dst));
        }

        // Add Scalar Double Precision Floating-Point Values
        constexpr void subsd(const XmmReg src, const XmmReg dst) {
            m_instructions.emplace_back(details::SubsdRR(src, dst));
        }

        constexpr void subsd(const Address& src, const XmmReg dst) {
            m_instructions.emplace_back(details::SubsdRM(src, dst));
        }

        // Unordered Compare Scalar Single Precision Floating-Point Values and Set EFLAGS
        constexpr void ucomiss(const XmmReg src, const XmmReg dst) {
            m_instructions.emplace_back(details::UcomissRR(src, dst));
        }

        constexpr void ucomiss(const Address& src, const XmmReg dst) {
            m_instructions.emplace_back(details::UcomissRM(src, dst));
        }

        // Unordered Compare Scalar Double Precision Floating-Point Values and Set EFLAGS
        constexpr void ucomisd(const XmmReg src, const XmmReg dst) {
            m_instructions.emplace_back(details::UcomisdRR(src, dst));
        }

        constexpr void ucomisd(const Address& src, const XmmReg dst) {
            m_instructions.emplace_back(details::UcomisdRM(src, dst));
        }

        // Compare Scalar Ordered Single Precision Floating-Point Values and Set EFLAGS
        constexpr void comiss(const XmmReg src, const XmmReg dst) {
            m_instructions.emplace_back(details::ComissRR(src, dst));
        }

        constexpr void comiss(const Address& src, const XmmReg dst) {
            m_instructions.emplace_back(details::ComissRM(src, dst));
        }

        // Compare Scalar Ordered Double Precision Floating-Point Values and Set EFLAGS
        constexpr void comisd(const XmmReg src, const XmmReg dst) {
            m_instructions.emplace_back(details::ComisdRR(src, dst));
        }

        constexpr void comisd(const Address& src, const XmmReg dst) {
            m_instructions.emplace_back(details::ComisdRM(src, dst));
        }

        // Bitwise Logical XOR of Packed Single Precision Floating-Point Values
        constexpr void xorps(const XmmReg src, const XmmReg dst) {
            m_instructions.emplace_back(details::XorpsRR(src, dst));
        }

        constexpr void xorps(const Address& src, const XmmReg dst) {
            m_instructions.emplace_back(details::XorpsRM(src, dst));
        }

        // Bitwise Logical XOR of Packed Double Precision Floating-Point Values
        constexpr void xorpd(const XmmReg src, const XmmReg dst) {
            m_instructions.emplace_back(details::XorpdRR(src, dst));
        }

        constexpr void xorpd(const Address& src, const XmmReg dst) {
            m_instructions.emplace_back(details::XorpdRM(src, dst));
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