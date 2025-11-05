#pragma once

#include "asm/x64/asm.h"

class MasmEmitter final {
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
        // if (src == 0) { m_asm.xor_(size, dst, dst); return; }
        m_asm.mov(size, src, dst);
    }

    void mov(const std::size_t size, const std::int64_t src, const aasm::GPReg dst) {
        m_asm.mov(size, src, dst);
    }

    template<typename Op>
    requires std::same_as<Op, aasm::GPReg> || std::is_same_v<Op, aasm::Address>
    void cmov(std::uint8_t size, aasm::CondType cond_type, const Op& src, aasm::GPReg dst) {
        m_asm.cmov(size, cond_type, src, dst);
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

    void add(const std::uint8_t size, const aasm::GPReg src, const aasm::Address& dst) {
        m_asm.add(size, src, dst);
    }

    template<GPVRegVariant Op>
    void add(const std::uint8_t size, const Op& src, const aasm::GPReg dst) {
        m_asm.add(size, src, dst);
    }

    void add(const std::uint8_t size, const std::int32_t src, const aasm::GPReg dst) {
        if (src == 0) {
            return; // No need to add zero
        }

        m_asm.add(size, src, dst);
    }

    void sub(const std::uint8_t size, const aasm::GPReg src, const aasm::Address& dst) {
        m_asm.sub(size, src, dst);
    }

    template<GPVRegVariant Op>
    void sub(const std::uint8_t size, const Op& src, const aasm::GPReg dst) {
        m_asm.sub(size, src, dst);
    }

    void sub(const std::uint8_t size, const std::int32_t src, const aasm::GPReg dst) {
        if (src == 0) {
            return;
        }

        m_asm.sub(size, src, dst);
    }

    void setcc(const aasm::CondType type, const aasm::GPReg dst) {
        m_asm.setcc(type, dst);
    }

    constexpr void lea(const aasm::Address& src, const aasm::GPReg dst) {
        m_asm.lea(src, dst);
    }

    template<typename Op>
    requires std::is_same_v<Op, aasm::GPReg> || std::is_same_v<Op, std::int32_t>
    void cmp(const std::uint8_t size, const Op& src, const aasm::GPReg dst) {
        m_asm.cmp(size, src, dst);
    }

    template<typename Op>
    requires std::is_same_v<Op, aasm::GPReg> || std::is_same_v<Op, std::int32_t>
    void cmp(const std::uint8_t size, const Op& src, const aasm::Address dst) {
        m_asm.cmp(size, src, dst);
    }

    template<typename Op>
    requires std::is_same_v<Op, aasm::GPReg> || std::is_same_v<Op, std::int32_t>
    void xxor(const std::uint8_t size, const Op& src, const aasm::GPReg dst) {
        m_asm.xxor(size, src, dst);
    }

    void xxor(const std::uint8_t size, const aasm::GPReg src, const aasm::Address& dst) {
        m_asm.xxor(size, src, dst);
    }

    void call(const aasm::Symbol* name) { m_asm.call(name); }
    void call(const aasm::Address& addr) { m_asm.call(addr); }

    // Move With Zero-Extend
    void movzx(const std::uint8_t src_size, const std::uint8_t dst_size, const aasm::GPReg src, const aasm::GPReg dst) {
        if (src_size == cst::DWORD_SIZE && dst_size == cst::QWORD_SIZE) {
            copy(src_size, src, dst);

        } else {
            m_asm.movzx(src_size, dst_size, src, dst);
        }
    }

    void movzx(const std::uint8_t src_size, const std::uint8_t dst_size, const aasm::Address& src, const aasm::GPReg dst) {
        if (src_size == cst::DWORD_SIZE && dst_size == cst::QWORD_SIZE) {
            m_asm.mov(src_size, src, dst);

        } else {
            m_asm.movzx(src_size, dst_size, src, dst);
        }
    }

    template<typename Op>
    requires std::is_same_v<Op, aasm::Address> || std::is_same_v<Op, aasm::GPReg>
    void movsx(const std::uint8_t src_size, const std::uint8_t dst_size, const Op& src, const aasm::GPReg dst) {
        if (src_size == cst::DWORD_SIZE && dst_size == cst::QWORD_SIZE) {
            m_asm.movsxd(src_size, dst_size, src, dst);

        } else {
            m_asm.movsx(src_size, dst_size, src, dst);
        }
    }

    // Two's Complement Negation
    template<typename Op>
    requires std::is_same_v<Op, aasm::GPReg> || std::is_same_v<Op, aasm::Address>
    constexpr void neg(const std::uint8_t size, const Op& r) {
        m_asm.neg(size, r);
    }

    // IDIV — Signed Divide
    template<typename Op>
    requires std::is_same_v<Op, aasm::GPReg> || std::is_same_v<Op, aasm::Address>
    constexpr void idiv(std::uint8_t size, const Op& r) {
        m_asm.idiv(size, r);
    }

    // DIV — Unsigned Divide
    template<typename Op>
    requires std::is_same_v<Op, aasm::GPReg> || std::is_same_v<Op, aasm::Address>
    constexpr void div(std::uint8_t size, const Op& r) {
        m_asm.div(size, r);
    }

    constexpr void movfp(const std::uint8_t size, const aasm::Address& src, const aasm::XmmReg dst) {
        switch (size) {
            case cst::DWORD_SIZE: m_asm.movss(src, dst); break;
            case cst::QWORD_SIZE: m_asm.movsd(src, dst); break;
            default: std::unreachable();
        }
    }

    constexpr void copyfp(const std::uint8_t size, const aasm::XmmReg src, const aasm::XmmReg dst) {
        if (src == dst) return;
        switch (size) {
            case cst::DWORD_SIZE: m_asm.movss(src, dst); break;
            case cst::QWORD_SIZE: m_asm.movsd(src, dst); break;
            default: std::unreachable();
        }
    }

    template<XVRegVariant Op>
    constexpr void addfp(const std::uint8_t size, const Op& src, const aasm::XmmReg dst) {
        switch (size) {
            case cst::DWORD_SIZE: m_asm.addss(src, dst); break;
            case cst::QWORD_SIZE: m_asm.addsd(src, dst); break;
            default: std::unreachable();
        }
    }

    template<XVRegVariant Op>
    constexpr void ucomis(const std::uint8_t size, const Op& src, const aasm::XmmReg dst) {
        switch (size) {
            case cst::DWORD_SIZE: m_asm.ucomisd(src, dst); break;
            case cst::QWORD_SIZE: m_asm.ucomiss(src, dst); break;
            default: std::unreachable();
        }
    }

    constexpr void cdq(const std::uint8_t size) {
        m_asm.cdq(size);
    }

    void leave() { m_asm.leave(); }

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
    aasm::AsmEmitter m_asm{};
};