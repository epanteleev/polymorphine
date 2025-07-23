#pragma once

#include <ostream>

#include "asm/Common.h"
#include "Label.h"
#include "Pop.h"
#include "Push.h"
#include "Mov.h"
#include "Ret.h"
#include "Add.h"
#include "Jmp.h"
#include "Cmp.h"
#include "CondType.h"
#include "Jcc.h"
#include "SetCC.h"

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
            PopR, PopM,
            PushR, PushM, PushI,
            Ret,
            MovRR, MovRI, MovMR, MovRM, MovMI,
            AddRR,
            CmpRR, CmpRI, CmpMI, CmpRM, CmpMR,
            Jmp, Jcc,
            SetCCR
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
