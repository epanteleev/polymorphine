#include "LIRBlock.h"

#include "utility/Error.h"
#include "lir/x64/instruction/LIRInstructionBase.h"

const LIRControlInstruction * LIRBlock::last() const {
    const auto inst = m_instructions.back();
    assertion(inst.has_value(), "last instruction is null");
    return dynamic_cast<LIRControlInstruction*>(inst.value());
}
