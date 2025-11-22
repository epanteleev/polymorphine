#pragma once

#include <iosfwd>
#include <variant>

#include "asm/x64/Common.h"
#include "asm/symbol/Symbol.h"
#include "asm/x64/encoding/Encoding.h"
#include "asm/x64/encoding/SSEEncoding.h"
#include "../../Label.h"
#include "asm/x64/reg/XmmReg.h"
#include "Pop.h"
#include "Push.h"
#include "Mov.h"
#include "Ret.h"
#include "Add.h"
#include "Sub.h"
#include "Jmp.h"
#include "Cmp.h"
#include "../CondType.h"
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
#include "Div.h"
#include "Cdq.h"
#include "Movss.h"
#include "Movsd.h"
#include "Addss.h"
#include "Subss.h"
#include "Ucomiss.h"
#include "Ucomisd.h"
#include "Comiss.h"
#include "Comisd.h"

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
        details::Leave,
        // SSE Instructions
        details::MovssRR, details::MovssRM,
        details::MovsdRR, details::MovsdRM,
        details::AddssRR, details::AddssRM,
        details::AddsdRR, details::AddsdRM,
        details::SubsdRR, details::SubsdRM,
        details::SubssRR, details::SubssRM,
        details::UcomisdRR, details::UcomisdRM,
        details::UcomissRR, details::UcomissRM,
        details::ComissRR, details::ComissRM,
        details::ComisdRR, details::ComisdRM
    >;

    std::ostream &operator<<(std::ostream &os, const X64Instruction &inst);
}