#pragma once

#include "asm/x64/asm.h"

class EmptyEmitter final {
public:
    void ret() {}

    template<typename Op>
    requires std::is_same_v<Op, aasm::GPReg> || std::is_same_v<Op, aasm::Address>
    void pop(const std::size_t, const Op&) {}

    template<typename Op>
    requires std::is_same_v<Op, aasm::GPReg> || std::is_same_v<Op, aasm::Address>
    void push(const std::size_t, const Op&) {}

    void copy(const std::size_t, const aasm::GPReg, const aasm::GPReg) {}

    void copy(const std::size_t, const std::int64_t, const aasm::GPReg) {}

    void mov(const std::size_t, const std::int64_t, const aasm::GPReg) {}

    template<typename Op>
    requires std::same_as<Op, aasm::GPReg> || std::is_same_v<Op, aasm::Address>
    void cmov(std::uint8_t, aasm::CondType, const Op&, aasm::GPReg) {}

    template<typename Op>
    requires std::is_same_v<Op, aasm::Address> || std::is_same_v<Op, std::int64_t>
    void mov(const std::uint8_t, const Op&, const aasm::GPReg&) {}

    template<typename Op>
    requires std::is_same_v<Op, aasm::GPReg> || std::is_same_v<Op, std::int32_t>
    void mov(const std::uint8_t, const Op&, const aasm::Address&) {}

    void add(const std::uint8_t, const aasm::GPReg, const aasm::Address&) {}

    template<typename Op>
    requires std::is_same_v<Op, aasm::GPReg> || std::is_same_v<Op, aasm::Address>
    void add(const std::uint8_t, const Op&, const aasm::GPReg) {}

    void add(const std::uint8_t size, const std::int32_t, const aasm::GPReg) {}

    void sub(const std::uint8_t, const aasm::GPReg, const aasm::Address&) {}

    template<typename Op>
    requires std::is_same_v<Op, aasm::GPReg> || std::is_same_v<Op, aasm::Address>
    void sub(const std::uint8_t, const Op&, const aasm::GPReg) {}

    void sub(const std::uint8_t, const std::int32_t, const aasm::GPReg) {}

    void setcc(const aasm::CondType, const aasm::GPReg) {}

    template<typename Op>
    requires std::is_same_v<Op, aasm::GPReg> || std::is_same_v<Op, std::int32_t>
    void cmp(const std::uint8_t, const Op&, const aasm::GPReg) {}

    template<typename Op>
    requires std::is_same_v<Op, aasm::GPReg> || std::is_same_v<Op, std::int32_t>
    void xxor(const std::uint8_t, const Op&, const aasm::GPReg) {}

    void xxor(const std::uint8_t, const aasm::GPReg, const aasm::Address&) {}

    void call(const aasm::Symbol*) { }
    void call(const aasm::Address&) { }

    void leave() { }

    void jmp(const aasm::Label&) {}

    void jcc(const aasm::CondType, const aasm::Label&) {}

    void set_label(const aasm::Label&) {}

private:
    aasm::AsmEmitter m_asm{};
};
