#include "MachBlock.h"
#include "AnyLIRInstruction.h"
#include "utility/Error.h"

LIRControlInstruction * MachBlock::last() const {
    const auto inst = m_instructions.back();
    if (!inst.has_value()) {
        die("no instruction found");
    }

    return dynamic_cast<LIRControlInstruction*>(inst.value());
}
