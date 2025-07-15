#pragma once

#include <ostream>

#include "utility/Error.h"
#include "asm/Common.h"
#include "Pop.h"
#include "Push.h"
#include "Mov.h"
#include "Ret.h"


namespace aasm {
    class X64Instruction final {
    public:
        template<typename I>
        explicit constexpr X64Instruction(I&& i) noexcept: m_inst(std::forward<I>(i)) {}

        friend std::ostream &operator<<(std::ostream &os, const X64Instruction &inst);

        template<CodeBuffer Buffer>
        constexpr void emit(Buffer &buffer) const {
            const auto visitor = [&](const auto &var) {
                var.emit(buffer);
            };

            std::visit(visitor, m_inst);
        }

    private:
        std::variant<
            PopR, PopM,
            PushR, PushM, PushI,
            Ret,
            MovRR, MovRI, MovMR, MovRM
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
