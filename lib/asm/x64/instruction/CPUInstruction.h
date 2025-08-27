#pragma once

#include <ostream>

#include "asm/x64/Common.h"
#include "asm/symbol/Symbol.h"
#include "asm/x64/Encoding.h"
#include "Label.h"
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
#include "CMov.h"
#include "Lea.h"
#include "Xor.h"
#include "Movzx.h"
#include "Movsx.h"
#include "Movsxd.h"

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
            details::Lea,
            details::PopR, details::PopM,
            details::PushR, details::PushM, details::PushI,
            details::Ret,
            details::CMovRR, details::CMovRM,
            details::MovRR, details::MovRI, details::MovMR, details::MovRM, details::MovMI,
            details::AddRR, details::AddRI, details::AddRM, details::AddMR, details::AddMI,
            details::SubRR, details::SubRI, details::SubRM, details::SubMI, details::SubMR,
            details::CmpRR, details::CmpRI, details::CmpMI, details::CmpRM, details::CmpMR,
            details::XorRR, details::XorRI, details::XorMI, details::XorRM, details::XorMR,
            details::MovzxRR, details::MovzxRM,
            details::MovsxRR, details::MovsxRM,
            details::MovsxdRR, details::MovsxdRM,
            details::Jmp, details::Jcc,
            details::SetCCR,
            details::Call, details::CallM,
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

    template<typename T>
    concept MemoryInstruction =
        std::same_as<T, details::Lea> ||
        std::same_as<T, details::CMovRM> ||
        std::same_as<T, details::MovzxRM> ||
        std::same_as<T, details::MovsxRM> ||
        std::same_as<T, details::MovsxdRM> ||
        std::same_as<T, details::Call> || std::same_as<T, details::CallM> ||
        std::same_as<T, details::PushM> || std::same_as<T, details::PopM> ||
        std::is_same_v<T, details::MovMR> || std::is_same_v<T, details::MovRM> || std::same_as<T, details::MovMI> ||
        std::is_same_v<T, details::AddMR> || std::is_same_v<T, details::AddRM> || std::is_same_v<T, details::AddMI> ||
        std::is_same_v<T, details::SubMR> || std::is_same_v<T, details::SubRM> || std::is_same_v<T, details::SubMI> ||
        std::is_same_v<T, details::CmpMR> || std::is_same_v<T, details::CmpRM> || std::is_same_v<T, details::CmpMI> ||
        std::is_same_v<T, details::XorMR> || std::is_same_v<T, details::XorRM> || std::is_same_v<T, details::XorMI>;
}
