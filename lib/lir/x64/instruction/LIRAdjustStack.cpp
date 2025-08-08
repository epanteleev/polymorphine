#include "LIRAdjustStack.h"

void LIRAdjustStack::visit(LIRVisitor &visitor) {
    switch (m_adjust_kind) {
        case LIRAdjustKind::UpStack: visitor.up_stack(m_caller_saved_regs, m_overflow_argument_area_size); break;
        case LIRAdjustKind::DownStack: visitor.down_stack(m_caller_saved_regs, m_overflow_argument_area_size); break;
        case LIRAdjustKind::Prologue: visitor.prologue(m_caller_saved_regs, m_overflow_argument_area_size); break;
        case LIRAdjustKind::Epilogue: visitor.epilogue(m_caller_saved_regs, m_overflow_argument_area_size); break;
        default: die("Unsupported LIRAdjustStack kind: {}", static_cast<int>(m_adjust_kind));
    }
}