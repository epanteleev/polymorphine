#pragma once

#include <ostream>

#include "asm/Common.h"
#include "Label.h"
#include "asm/Encoding.h"
#include "Pop.h"
#include "Push.h"
#include "Mov.h"
#include "Ret.h"
#include "Add.h"
#include "Sub.h"
#include "Jmp.h"
#include "Cmp.h"
#include "CondType.h"
#include "Jcc.h"
#include "SetCC.h"
#include "Leave.h"
#include "Call.h"

namespace aasm {
    class X64Instruction final {
    public:
        template<typename I>
        explicit constexpr X64Instruction(I&& i) noexcept: m_inst(std::forward<I>(i)) {}

        friend std::ostream &operator<<(std::ostream &os, const X64Instruction &inst);

        template<typename Fn>
        constexpr void visit(Fn&& fn) const {
            std::visit(fn, m_inst);
        }

    private:
        std::variant<
            details::PopR, details::PopM,
            details::PushR, details::PushM, details::PushI,
            details::Ret,
            details::MovRR, details::MovRI, details::MovMR, details::MovRM, details::MovMI,
            details::AddRR, details::AddRI, details::AddRM, details::AddMR, details::AddMI,
            details::SubRR, details::SubRI, details::SubRM, details::SubMI,
            details::CmpRR, details::CmpRI, details::CmpMI, details::CmpRM, details::CmpMR,
            details::Jmp, details::Jcc,
            details::SetCCR,
            details::Call,
            details::Leave
        > m_inst;
    };

    inline std::ostream &operator<<(std::ostream &os, const X64Instruction &inst) {
        const auto visitor = [&](const auto &var) {
            os << var;
        };

        std::visit(visitor, inst.m_inst);
        return os;
    }
}
