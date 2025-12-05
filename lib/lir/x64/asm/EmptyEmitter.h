#pragma once

#include "ShiftKind.h"
#include "lir/x64/asm/FcmpOrdering.h"
#include "lir/x64/asm/operand/XVReg.h"
#include "asm/x64/asm.h"
#include "lir/x64/asm/operand/Constrains.h"

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

    template<GPVRegVariant Op>
    void shift(const std::uint8_t, const ShiftKind, const Op&) {}

    template<GPVRegVariant Op>
    void shift(const std::uint8_t, const ShiftKind, std::size_t, const Op&) {}

    void add(const std::uint8_t, const aasm::GPReg, const aasm::Address&) {}

    template<typename Op>
    requires std::is_same_v<Op, aasm::GPReg> || std::is_same_v<Op, aasm::Address>
    void add(const std::uint8_t, const Op&, const aasm::GPReg) {}

    void add(const std::uint8_t, const std::int32_t, const aasm::GPReg) {}

    void sub(const std::uint8_t, const aasm::GPReg, const aasm::Address&) {}

    template<typename Op>
    requires std::is_same_v<Op, aasm::GPReg> || std::is_same_v<Op, aasm::Address>
    void sub(const std::uint8_t, const Op&, const aasm::GPReg) {}

    void sub(const std::uint8_t, const std::int32_t, const aasm::GPReg) {}

    void setcc(const aasm::CondType, const aasm::GPReg) {}

    void lea(const aasm::Address&, const aasm::GPReg) {}

    template<typename Op>
    requires std::is_same_v<Op, aasm::GPReg> || std::is_same_v<Op, std::int32_t>
    void cmp(const std::uint8_t, const Op&, const aasm::GPReg) {}

    template<typename Op>
    requires std::is_same_v<Op, aasm::GPReg> || std::is_same_v<Op, std::int32_t>
    void cmp(const std::uint8_t, const Op&, const aasm::Address) {}

    template<typename Op>
    requires std::is_same_v<Op, aasm::GPReg> || std::is_same_v<Op, std::int32_t>
    void xxor(const std::uint8_t, const Op&, const aasm::GPReg) {}

    void xxor(const std::uint8_t, const aasm::GPReg, const aasm::Address&) {}

    void call(const aasm::Symbol*) { }
    void call(const aasm::Address&) { }

    template<typename Op>
    requires std::is_same_v<Op, aasm::Address> || std::is_same_v<Op, aasm::GPReg>
    void movzx(const std::uint8_t, const std::uint8_t, const Op &,const aasm::GPReg) {}

    template<typename Op>
    requires std::is_same_v<Op, aasm::Address> || std::is_same_v<Op, aasm::GPReg>
    void movsx(const std::uint8_t, const std::uint8_t, const Op &, const aasm::GPReg) {}

    template<typename Op>
    requires std::is_same_v<Op, aasm::GPReg> || std::is_same_v<Op, aasm::Address>
    constexpr void neg(const std::uint8_t, const Op &) {}

    // IDIV — Signed Divide
    template<typename Op>
    requires std::is_same_v<Op, aasm::GPReg> || std::is_same_v<Op, aasm::Address>
    constexpr void idiv(std::uint8_t, const Op&) {}

    // DIV — Unsigned Divide
    template<typename Op>
    requires std::is_same_v<Op, aasm::GPReg> || std::is_same_v<Op, aasm::Address>
    constexpr void div(std::uint8_t, const Op&) {}

    constexpr void copyfp(const std::uint8_t, const aasm::XmmReg, const aasm::XmmReg) {}

    constexpr void movfp(const std::uint8_t, const aasm::Address&, const aasm::XmmReg) {}
    constexpr void movfp(const std::uint8_t, const aasm::XmmReg&, const aasm::Address&) {}

    template<typename Op>
    requires std::is_same_v<Op, aasm::XmmReg> || std::is_same_v<Op, aasm::Address>
    constexpr void addfp(const std::uint8_t, const Op&, const aasm::XmmReg) {}

    template<XVRegVariant Op>
    constexpr void cmpfp(const FcmpOrdering, const std::uint8_t, const Op&, const aasm::XmmReg) {}

    template<XVRegVariant Op>
    constexpr void xorfp(const std::uint8_t, const Op&, const aasm::XmmReg) {}

    template<XVRegVariant Op>
    constexpr void cvtfp2si(const std::uint8_t, const std::uint8_t, const Op, const aasm::GPReg) {}

    template<GPVRegVariant Op>
    constexpr void cvtsi2fp(const std::uint8_t, const std::uint8_t, const Op&, const aasm::XmmReg) {}

    constexpr void cdq(const std::uint8_t) {}
    void leave() { }

    void jmp(const aasm::Label&) {}

    void jcc(const aasm::CondType, const aasm::Label&) {}

    void set_label(const aasm::Label&) {}
};