#include "MachBlock.h"

#include <ostream>

#include "LIRInstructionBase.h"
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
    std::size_t count = 0;
    const auto size = m_instructions.size();
    for (auto& inst: m_instructions) {
        os << "  ";
        inst.print(os);

        if (count != size-1) {
            os << std::endl;
        }

        count += 1;
    }
}
