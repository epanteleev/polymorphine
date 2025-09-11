#pragma once

#include <ostream>
#include <variant>

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
#include "Neg.h"
#include "Idiv.h"
#include "Div.h"
#include "Cdq.h"

namespace aasm {
    using X64Instruction = std::variant<
        details::Cdq,
        details::Lea,
        details::PopR, details::PopM,
        details::NegR, details::NegM,
        details::IdivR, details::IdivM,
        details::UDivR, details::UDivM,
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
    >;

    inline std::ostream &operator<<(std::ostream &os, const X64Instruction &inst) {
        const auto visitor = [&](const auto &var) {
            os << var;
        };

        std::visit(visitor, inst);
        return os;
    }

    template<typename T>
    concept MemoryInstruction =
        std::same_as<T, details::Lea> ||
        std::same_as<T, details::CMovRM> ||
        std::same_as<T, details::MovzxRM> ||
        std::same_as<T, details::MovsxRM> ||
        std::same_as<T, details::MovsxdRM> ||
        std::same_as<T, details::NegM> ||
        std::same_as<T, details::IdivM> ||
        std::same_as<T, details::UDivM> ||
        std::same_as<T, details::Call> || std::same_as<T, details::CallM> ||
        std::same_as<T, details::PushM> || std::same_as<T, details::PopM> ||
        std::is_same_v<T, details::MovMR> || std::is_same_v<T, details::MovRM> || std::same_as<T, details::MovMI> ||
        std::is_same_v<T, details::AddMR> || std::is_same_v<T, details::AddRM> || std::is_same_v<T, details::AddMI> ||
        std::is_same_v<T, details::SubMR> || std::is_same_v<T, details::SubRM> || std::is_same_v<T, details::SubMI> ||
        std::is_same_v<T, details::CmpMR> || std::is_same_v<T, details::CmpRM> || std::is_same_v<T, details::CmpMI> ||
        std::is_same_v<T, details::XorMR> || std::is_same_v<T, details::XorRM> || std::is_same_v<T, details::XorMI>;
}