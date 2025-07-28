#include "LIRBlock.h"

#include <ostream>

#include "../instruction/LIRInstructionBase.h"
#include "utility/Error.h"

LIRControlInstruction * LIRBlock::last() const {
    const auto inst = m_instructions.back();
    assertion(inst.has_value(), "last instruction is null");
    return dynamic_cast<LIRControlInstruction*>(inst.value());
}
