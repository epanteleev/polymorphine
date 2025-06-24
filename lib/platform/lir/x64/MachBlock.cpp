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

void MachBlock::print(std::ostream &os) const {
    os << m_id << ":" << std::endl;
    for (auto& inst: m_instructions) {
        inst.print(os);
        os << std::endl;
    }
}
