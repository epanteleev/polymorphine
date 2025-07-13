#include "LIRBlock.h"

#include <ostream>

#include "../instruction/LIRInstructionBase.h"
#include "utility/Error.h"

LIRControlInstruction * LIRBlock::last() const {
    const auto inst = m_instructions.back();
    if (!inst.has_value()) {
        die("no instruction found");
    }

    return dynamic_cast<LIRControlInstruction*>(inst.value());
}
