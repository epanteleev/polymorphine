#include "LIRAdjustStack.h"

void LIRAdjustStack::visit(LIRVisitor &visitor) {
    switch (m_adjust_kind) {
        case LIRAdjustKind::UpStack: visitor.up_stack(m_caller_saved_regs, m_stack_size); break;
        case LIRAdjustKind::DownStack: visitor.down_stack(m_caller_saved_regs, m_stack_size); break;
        default: die("Unsupported LIRAdjustStack kind: {}", static_cast<int>(m_adjust_kind));
    }
}