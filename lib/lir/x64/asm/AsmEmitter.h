#pragma once

#include "asm/Address.h"
#include "asm/Assembler.h"

class AsmEmitter final {
public:
    void ret() {
        m_asm.ret();
    }

    template<typename Op>
    requires std::is_same_v<Op, aasm::GPReg> || std::is_same_v<Op, aasm::Address>
    void pop(const std::size_t size, const Op& r) {
        m_asm.pop(size, r);
    }

    template<typename Op>
    requires std::is_same_v<Op, aasm::GPReg> || std::is_same_v<Op, aasm::Address>
    void push(const std::size_t size, const Op& r) {
        m_asm.push(size, r);
    }

    void copy(const std::size_t size, const aasm::GPReg src, const aasm::GPReg dst) {
        if (src == dst) {
            return;
        }

        m_asm.mov(size, src, dst);
    }

    void copy(const std::size_t size, const std::int64_t src, const aasm::GPReg dst) {
        m_asm.mov(size, src, dst);
    }

    template<typename Op>
    requires std::is_same_v<Op, aasm::Address> || std::is_same_v<Op, std::int64_t>
    void mov(const std::uint8_t size, const Op& src, const aasm::GPReg& dst) {
        m_asm.mov(size, src, dst);
    }

    template<typename Op>
    requires std::is_same_v<Op, aasm::GPReg> || std::is_same_v<Op, std::int32_t>
    void mov(const std::uint8_t size, const Op& src, const aasm::Address& dst) {
        m_asm.mov(size, src, dst);
    }

    template<typename Op>
    void add(const std::uint8_t size, const aasm::GPReg src, const aasm::Address& dst) {
        m_asm.add(size, src, dst);
    }

    template<typename Op>
    requires std::is_same_v<Op, aasm::GPReg> || std::is_same_v<Op, std::int32_t> || std::is_same_v<Op, aasm::Address>
    void add(const std::uint8_t size, const Op& src, const aasm::GPReg dst) {
        m_asm.add(size, src, dst);
    }

    void setcc(const aasm::CondType type, const aasm::GPReg dst) {
        m_asm.setcc(type, dst);
    }

    template<typename Op>
    requires std::is_same_v<Op, aasm::GPReg> || std::is_same_v<Op, std::int32_t>
    void cmp(const std::uint8_t size, const Op& src, const aasm::GPReg dst) {
        m_asm.cmp(size, src, dst);
    }

    void leave() {
        m_asm.leave();
    }

    void jmp(const aasm::Label& label) {
        m_asm.jmp(label);
    }

    void jcc(const aasm::CondType type, const aasm::Label& label) {
        m_asm.jcc(type, label);
    }

    [[nodiscard]]

    aasm::Label create_label() {
        return m_asm.create_label();
    }

    void set_label(const aasm::Label& label) {
        m_asm.set_label(label);
    }

    aasm::AsmBuffer to_buffer() noexcept {
        return m_asm.to_buffer();
    }

    [[nodiscard]]
    std::size_t size() const noexcept {
        return m_asm.size();
    }

private:
    aasm::Assembler m_asm{};
};
